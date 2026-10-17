#include "ip.hpp"
#include "routing.hpp"

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

    std::cout << "All CCNA tests passed.\n";
}
