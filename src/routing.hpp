#pragma once

#include "ip.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ccna {

struct Route {
  Cidr dest;            // destination prefix
  std::string next_hop; // could be an IP string or interface name
  int metric{};         // optional
};

// Longest Prefix Match: return index of best matching route in `routes`.
// Returns -1 if no route matches.
int lpm_lookup(const std::vector<Route>& routes, std::uint32_t dest_ip);

} // namespace ccna

