#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace ccna {

struct IPv6Address {
  std::array<std::uint8_t, 16> bytes{};

  friend bool operator==(const IPv6Address&, const IPv6Address&) = default;
};

// Parse a canonical or compressed IPv6 address. Embedded IPv4 dotted-decimal
// notation is accepted in the final 32 bits (for example ::ffff:192.0.2.1).
std::optional<IPv6Address> parse_ipv6(const std::string& text);

// Format an IPv6 address using RFC 5952-style lowercase hexadecimal and ::
// compression where appropriate.
std::string format_ipv6(const IPv6Address& address);

// Build a 128-bit prefix mask represented as an IPv6 address.
// prefix_len must be in [0,128].
IPv6Address mask_from_ipv6_prefix(int prefix_len);

struct IPv6Cidr {
  IPv6Address network{};
  IPv6Address last{};
  int prefix_len{};
};

// Build an IPv6 CIDR range, aligning the supplied address to the prefix.
IPv6Cidr make_ipv6_cidr(const IPv6Address& address, int prefix_len);

// Test whether an address belongs to a CIDR prefix.
bool ipv6_contains(const IPv6Cidr& cidr, const IPv6Address& address);

} // namespace ccna
