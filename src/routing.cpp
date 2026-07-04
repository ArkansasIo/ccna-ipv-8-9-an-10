#include "routing.hpp"

namespace ccna {

int lpm_lookup(const std::vector<Route>& routes, std::uint32_t dest_ip) {
  int best_idx = -1;
  int best_prefix = -1;

  for (int i = 0; i < static_cast<int>(routes.size()); ++i) {
    const auto& r = routes[i];
    const auto& c = r.dest;

    // Check match: dest_ip within [network, broadcast]
    if (dest_ip >= c.network && dest_ip <= c.broadcast) {
      if (c.prefix_len > best_prefix) {
        best_prefix = c.prefix_len;
        best_idx = i;
      }
    }
  }

  return best_idx;
}

} // namespace ccna

