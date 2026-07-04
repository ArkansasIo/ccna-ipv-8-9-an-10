#include "ip.hpp"

#include <sstream>

namespace ccna {

std::optional<std::uint32_t> parse_ipv4(const std::string& dotted) {
  std::stringstream ss(dotted);
  std::string part;
  std::vector<int> octets;

  while (std::getline(ss, part, '.')) {
    if (part.empty()) return std::nullopt;
    for (char ch : part) {
      if (ch < '0' || ch > '9') return std::nullopt;
    }
    int v = 0;
    try {
      v = std::stoi(part);
    } catch (...) {
      return std::nullopt;
    }
    if (v < 0 || v > 255) return std::nullopt;
    octets.push_back(v);
  }

  if (octets.size() != 4) return std::nullopt;

  std::uint32_t ip = 0;
  ip |= (static_cast<std::uint32_t>(octets[0]) << 24);
  ip |= (static_cast<std::uint32_t>(octets[1]) << 16);
  ip |= (static_cast<std::uint32_t>(octets[2]) << 8);
  ip |= (static_cast<std::uint32_t>(octets[3]));
  return ip;
}

std::string format_ipv4(std::uint32_t ip) {
  std::ostringstream out;
  out << ((ip >> 24) & 0xFF) << '.'
      << ((ip >> 16) & 0xFF) << '.'
      << ((ip >> 8) & 0xFF) << '.'
      << (ip & 0xFF);
  return out.str();
}

std::uint32_t mask_from_prefix(int prefix_len) {
  // Repo currently implements IPv4 math only (32-bit addresses).
  // If caller provides p > 32 (e.g. 64/96/128/256), we intentionally reject.
  if (prefix_len < 0 || prefix_len > 32) {
    throw std::invalid_argument("prefix_len must be in [0,32] for IPv4 CIDR math");
  }
  if (prefix_len == 0) return 0u;
  // Example prefix=24 => mask = 11111111 11111111 11111111 00000000
  return 0xFFFFFFFFu << (32 - prefix_len);
}


Cidr make_cidr(std::uint32_t ip, int prefix_len) {
  std::uint32_t mask = mask_from_prefix(prefix_len);
  std::uint32_t network = ip & mask;
  std::uint32_t broadcast = network | (~mask);
  return Cidr{network, broadcast, prefix_len};
}

HostRange usable_host_range(const Cidr& c) {
  // Classic assumption: network and broadcast are not usable.
  if (c.prefix_len >= 31) {
    return HostRange{std::nullopt, std::nullopt};
  }
  std::uint32_t first = c.network + 1;
  std::uint32_t last = c.broadcast - 1;
  return HostRange{first, last};
}

} // namespace ccna

