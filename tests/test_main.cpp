#include "ip.hpp"
#include "ipv6.hpp"
#include "ipv6_routing.hpp"
#include "routing.hpp"
#include "vlsm.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void expect(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}
}

int main() {
  using namespace ccna;

  expect(parse_ipv4("192.168.1.10").has_value(), "valid IPv4 parses");
  expect(!parse_ipv4("192.168.1").has_value(), "too few octets rejected");
  expect(!parse_ipv4("192.168.1.256").has_value(), "octet above 255 rejected");
  expect(!parse_ipv4("192.168.-1.1").has_value(), "negative octet rejected");
  expect(!parse_ipv4("1.2.3.4.5").has_value(), "too many octets rejected");
  expect(!parse_ipv4("1..3.4").has_value(), "empty octet rejected");

  const auto ip = *parse_ipv4("192.168.10.5");
  const auto cidr = make_cidr(ip, 24);
  expect(format_ipv4(cidr.network) == "192.168.10.0", "/24 network");
  expect(format_ipv4(cidr.broadcast) == "192.168.10.255", "/24 broadcast");
  expect(format_ipv4(mask_from_prefix(24)) == "255.255.255.0", "/24 mask");
  expect(usable_host_range(cidr).first == parse_ipv4("192.168.10.1"), "/24 first host");
  expect(usable_host_range(cidr).last == parse_ipv4("192.168.10.254"), "/24 last host");
  expect(format_ipv4(make_cidr(*parse_ipv4("1.2.3.4"), 0).network) == "0.0.0.0", "/0 network");
  expect(format_ipv4(make_cidr(*parse_ipv4("1.2.3.4"), 32).network) == "1.2.3.4", "/32 network");

  bool rejected = false;
  try { (void)mask_from_prefix(33); } catch (const std::invalid_argument&) { rejected = true; }
  expect(rejected, "prefix above 32 rejected");
  rejected = false;
  try { (void)mask_from_prefix(-1); } catch (const std::invalid_argument&) { rejected = true; }
  expect(rejected, "negative prefix rejected");

  std::vector<Route> routes{
      {make_cidr(*parse_ipv4("10.0.0.0"), 8), "A", 10},
      {make_cidr(*parse_ipv4("10.1.0.0"), 16), "B", 10},
      {make_cidr(*parse_ipv4("10.1.2.0"), 24), "C", 10},
      {make_cidr(*parse_ipv4("0.0.0.0"), 0), "DEFAULT", 100}
  };
  auto idx = lpm_lookup(routes, *parse_ipv4("10.1.2.55"));
  expect(idx >= 0 && routes[static_cast<std::size_t>(idx)].next_hop == "C", "longest prefix selected");
  idx = lpm_lookup(routes, *parse_ipv4("10.1.88.1"));
  expect(idx >= 0 && routes[static_cast<std::size_t>(idx)].next_hop == "B", "/16 selected");
  idx = lpm_lookup(routes, *parse_ipv4("8.8.8.8"));
  expect(idx >= 0 && routes[static_cast<std::size_t>(idx)].next_hop == "DEFAULT", "default route selected");
  idx = lpm_lookup({}, *parse_ipv4("8.8.8.8"));
  expect(idx == -1, "empty table produces no route");

  expect(prefix_for_hosts(1) == 30, "one host uses /30 under classic rules");
  expect(prefix_for_hosts(2) == 30, "two hosts use /30 under classic rules");
  expect(prefix_for_hosts(62) == 26, "62 hosts use /26");
  expect(prefix_for_hosts(254) == 24, "254 hosts use /24");
  rejected = false;
  try { (void)prefix_for_hosts(0); } catch (const std::invalid_argument&) { rejected = true; }
  expect(rejected, "zero-host VLSM requirement rejected");

  const auto allocations = allocate_vlsm(
      *parse_ipv4("192.168.10.0"), 24,
      {{120, "A"}, {60, "B"}, {30, "C"}, {10, "D"}});
  expect(allocations.size() == 4, "four VLSM allocations created");
  expect(format_ipv4(allocations[0].cidr.network) == "192.168.10.0", "VLSM A network");
  expect(allocations[0].prefix_len == 25, "VLSM A prefix");
  expect(format_ipv4(allocations[1].cidr.network) == "192.168.10.128", "VLSM B network");
  expect(allocations[1].prefix_len == 26, "VLSM B prefix");
  expect(format_ipv4(allocations[2].cidr.network) == "192.168.10.192", "VLSM C network");
  expect(allocations[2].prefix_len == 27, "VLSM C prefix");
  expect(format_ipv4(allocations[3].cidr.network) == "192.168.10.224", "VLSM D network");
  expect(allocations[3].prefix_len == 28, "VLSM D prefix");

  rejected = false;
  try {
    (void)allocate_vlsm(*parse_ipv4("192.168.10.0"), 28, {{20, "too-large"}});
  } catch (const std::runtime_error&) {
    rejected = true;
  }
  expect(rejected, "VLSM detects parent exhaustion");

  const auto v6 = *parse_ipv6("2001:0DB8:0000:0000:0000:0000:0000:0042");
  expect(format_ipv6(v6) == "2001:db8::42", "IPv6 RFC 5952 formatting");
  expect(parse_ipv6("2001:db8::42") == v6, "compressed IPv6 parses");
  expect(parse_ipv6("::1").has_value(), "IPv6 loopback parses");
  expect(parse_ipv6("::ffff:192.0.2.1").has_value(), "IPv4-mapped IPv6 parses");
  expect(!parse_ipv6("2001:::1").has_value(), "invalid IPv6 compression rejected");
  expect(format_ipv6(make_ipv6_cidr(v6, 64).network) == "2001:db8::", "IPv6 /64 network");
  expect(format_ipv6(make_ipv6_cidr(v6, 64).last) == "2001:db8:0:0:ffff:ffff:ffff:ffff", "IPv6 /64 last address");

  rejected = false;
  try { (void)mask_from_ipv6_prefix(129); } catch (const std::invalid_argument&) { rejected = true; }
  expect(rejected, "IPv6 prefix above 128 rejected");

  const auto v6_32 = make_ipv6_cidr(*parse_ipv6("2001:db8::1"), 32);
  const auto v6_48 = make_ipv6_cidr(*parse_ipv6("2001:db8:1234::1"), 48);
  const auto v6_64 = make_ipv6_cidr(*parse_ipv6("2001:db8:1234:5678::1"), 64);
  std::vector<IPv6Route> v6_routes{
      {make_ipv6_cidr(*parse_ipv6("::"), 0), "DEFAULT", 100},
      {v6_32, "CORE", 20},
      {v6_48, "DIST", 10},
      {v6_64, "ACCESS", 5}
  };
  idx = ipv6_lpm_lookup(v6_routes, *parse_ipv6("2001:db8:1234:5678::42"));
  expect(idx >= 0 && v6_routes[static_cast<std::size_t>(idx)].next_hop == "ACCESS", "IPv6 longest prefix selected");
  idx = ipv6_lpm_lookup(v6_routes, *parse_ipv6("2001:db8:9999::42"));
  expect(idx >= 0 && v6_routes[static_cast<std::size_t>(idx)].next_hop == "CORE", "IPv6 /32 selected");
  idx = ipv6_lpm_lookup(v6_routes, *parse_ipv6("2001:4860:4860::8888"));
  expect(idx >= 0 && v6_routes[static_cast<std::size_t>(idx)].next_hop == "DEFAULT", "IPv6 default selected");

  std::cout << "All CCNA tests passed.\n";
}
