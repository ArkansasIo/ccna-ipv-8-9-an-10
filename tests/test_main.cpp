#include "ip.hpp"
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

  std::cout << "All CCNA tests passed.\n";
}
