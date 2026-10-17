#pragma once

#include "ipv6.hpp"

#include <string>
#include <vector>

namespace ccna {

struct IPv6Route {
  IPv6Cidr dest;
  std::string next_hop;
  int metric{};
};

// Longest-prefix match for IPv6. Returns the index of the best route or -1.
int ipv6_lpm_lookup(const std::vector<IPv6Route>& routes, const IPv6Address& destination);

} // namespace ccna
