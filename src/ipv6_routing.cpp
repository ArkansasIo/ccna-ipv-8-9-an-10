#include "ipv6_routing.hpp"

namespace ccna {

int ipv6_lpm_lookup(const std::vector<IPv6Route>& routes, const IPv6Address& destination) {
  int best = -1;
  int best_prefix = -1;
  int best_metric = 0;

  for (std::size_t i = 0; i < routes.size(); ++i) {
    const auto& route = routes[i];
    if (!ipv6_contains(route.dest, destination)) continue;

    if (route.dest.prefix_len > best_prefix ||
        (route.dest.prefix_len == best_prefix &&
         (best < 0 || route.metric < best_metric))) {
      best = static_cast<int>(i);
      best_prefix = route.dest.prefix_len;
      best_metric = route.metric;
    }
  }
  return best;
}

} // namespace ccna
