#include "vlsm.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace ccna;

int main() {
  const std::string parent_str = "192.168.10.0/24";
  const auto slash = parent_str.find('/');
  const auto ip_str = parent_str.substr(0, slash);
  const int parent_prefix = std::stoi(parent_str.substr(slash + 1));
  const auto parsed = parse_ipv4(ip_str);

  if (!parsed) {
    std::cerr << "Invalid parent network\n";
    return 1;
  }

  const std::vector<VlsmRequirement> requirements = {
      {120, "net-A"},
      {60, "net-B"},
      {30, "net-C"},
      {10, "net-D"},
  };

  try {
    const auto allocations = allocate_vlsm(*parsed, parent_prefix, requirements);

    for (const auto& allocation : allocations) {
      std::cout << allocation.name << ": "
                << format_ipv4(allocation.cidr.network) << "/"
                << allocation.prefix_len
                << "  broadcast=" << format_ipv4(allocation.cidr.broadcast);

      if (allocation.usable.first && allocation.usable.last) {
        std::cout << "  usable="
                  << format_ipv4(*allocation.usable.first) << "-"
                  << format_ipv4(*allocation.usable.last);
      }
      std::cout << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Allocation error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
