#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace ccna {

// Parse dotted-decimal IPv4 (e.g. "192.168.1.10") into uint32.
// Returns std::nullopt if invalid.
std::optional<std::uint32_t> parse_ipv4(const std::string& dotted);

// Convert uint32 IPv4 back to dotted-decimal string.
std::string format_ipv4(std::uint32_t ip);

// CIDR mask from prefix length (IPv4-only => prefix_len in [0,32]).
std::uint32_t mask_from_prefix(int prefix_len);


struct Cidr {
  std::uint32_t network{};
  std::uint32_t broadcast{};
  int prefix_len{};
};

// Build CIDR info from base network and prefix.
// Assumes base is an IPv4 address (not necessarily aligned); alignment is applied.
Cidr make_cidr(std::uint32_t ip, int prefix_len);

// Compute usable host range (classic assumption: network/broadcast not usable).
// For /31 and /32, behavior varies; we return empty ranges where unusable.
struct HostRange {
  std::optional<std::uint32_t> first;
  std::optional<std::uint32_t> last;
};

HostRange usable_host_range(const Cidr& c);

} // namespace ccna

