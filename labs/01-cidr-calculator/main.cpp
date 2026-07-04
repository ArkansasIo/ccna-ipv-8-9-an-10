#include "../../src/ip.hpp"

#include <iostream>
#include <string>

using namespace ccna;

static bool parse_ip_prefix(const std::string& in, std::uint32_t& ip, int& prefix) {
  auto slash = in.find('/');
  if (slash == std::string::npos) return false;
  auto ip_str = in.substr(0, slash);
  auto p_str = in.substr(slash + 1);
  int p = 0;
  try {
    p = std::stoi(p_str);
  } catch (...) {
    return false;
  }
  auto parsed = parse_ipv4(ip_str);
  if (!parsed) return false;
  ip = *parsed;
  prefix = p;
  return true;
}

int main() {
  std::string input = "192.168.10.5/24"; // change if desired

  std::uint32_t ip = 0;
  int prefix = 0;
  if (!parse_ip_prefix(input, ip, prefix)) {
    std::cerr << "Invalid input format. Expected ip/prefix\n";
    return 1;
  }

  auto cidr = make_cidr(ip, prefix);
  auto hosts = usable_host_range(cidr);

  std::uint32_t mask = mask_from_prefix(prefix);

  std::cout << "Input: " << input << "\n";
  std::cout << "Mask:  " << format_ipv4(mask) << "\n";
  std::cout << "Network: " << format_ipv4(cidr.network) << "/" << cidr.prefix_len << "\n";
  std::cout << "Broadcast: " << format_ipv4(cidr.broadcast) << "\n";
  if (hosts.first && hosts.last) {
    std::cout << "Usable: " << format_ipv4(*hosts.first) << " - " << format_ipv4(*hosts.last) << "\n";
  } else {
    std::cout << "Usable: (none for /31 or /32 under classic assumption)\n";
  }
}

