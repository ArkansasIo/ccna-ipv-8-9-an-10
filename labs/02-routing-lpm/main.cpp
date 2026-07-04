#include "../../src/ip.hpp"
#include "../../src/routing.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace ccna;

int main() {
  // Define routing table.
  std::vector<Route> routes;
  
  // 10.0.0.0/8  -> next hop A
  {
    auto net = parse_ipv4("10.0.0.0").value();
    routes.push_back(Route{make_cidr(net, 8), "NextHop-A", 1});
  }
  // 10.1.0.0/16 -> next hop B
  {
    auto net = parse_ipv4("10.1.0.0").value();
    routes.push_back(Route{make_cidr(net, 16), "NextHop-B", 1});
  }
  // 10.1.2.0/24 -> next hop C
  {
    auto net = parse_ipv4("10.1.2.0").value();
    routes.push_back(Route{make_cidr(net, 24), "NextHop-C", 1});
  }

  struct Test { std::string dst; };
  std::vector<Test> tests = {
      {"10.1.2.55"},
      {"10.1.88.1"},
      {"10.22.1.1"},
  };

  for (const auto& t : tests) {
    auto dst_ip = parse_ipv4(t.dst);
    if (!dst_ip) {
      std::cerr << "Invalid dst: " << t.dst << "\n";
      continue;
    }

    int best = lpm_lookup(routes, *dst_ip);
    std::cout << "DST " << t.dst << " -> ";
    if (best < 0) {
      std::cout << "NO ROUTE\n";
    } else {
      std::cout << routes[best].next_hop << " (matched /"
                << routes[best].dest.prefix_len << ")\n";
    }
  }

  return 0;
}

