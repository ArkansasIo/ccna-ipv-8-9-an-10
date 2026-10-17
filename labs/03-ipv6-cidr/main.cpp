#include "ipv6.hpp"
#include "ipv6_routing.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace ccna;

int main() {
  const auto address = parse_ipv6("2001:db8:1234:5678::42");
  if (!address) {
    std::cerr << "Invalid IPv6 address\n";
    return 1;
  }

  try {
    const auto cidr = make_ipv6_cidr(*address, 64);
    std::cout << "address=" << format_ipv6(*address) << "\n"
              << "network=" << format_ipv6(cidr.network) << "/" << cidr.prefix_len << "\n"
              << "last=" << format_ipv6(cidr.last) << "\n";

    const auto probe = parse_ipv6("2001:db8:1234:5678::abcd");
    if (probe) {
      std::cout << "probe-in-subnet=" << (ipv6_contains(cidr, *probe) ? "yes" : "no") << "\n";
    }

    const auto default_cidr = make_ipv6_cidr(*address, 0);
    const std::vector<IPv6Route> routes = {
        {default_cidr, "uplink0", 100},
        {make_ipv6_cidr(*parse_ipv6("2001:db8::1"), 32), "core0", 20},
        {make_ipv6_cidr(*parse_ipv6("2001:db8:1234::1"), 48), "dist0", 10},
        {cidr, "access0", 5},
    };

    const int selected = ipv6_lpm_lookup(routes, *address);
    if (selected >= 0) {
      std::cout << "best-next-hop=" << routes[static_cast<std::size_t>(selected)].next_hop << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "IPv6 error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
