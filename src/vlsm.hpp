#pragma once

#include "ip.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ccna {

struct VlsmRequirement {
  int hosts;
  std::string name;
};

struct VlsmAllocation {
  std::string name;
  int requested_hosts{};
  int prefix_len{};
  Cidr cidr{};
  HostRange usable{};
};

// Return the smallest traditional IPv4 CIDR block whose usable host count
// is at least `hosts`. Uses the classic network/broadcast exclusion rule.
int prefix_for_hosts(int hosts);

// Allocate requirements from a parent network using largest-block-first VLSM.
// Throws std::invalid_argument for invalid input and std::runtime_error when
// the requested blocks do not fit inside the parent network.
std::vector<VlsmAllocation> allocate_vlsm(
    std::uint32_t parent_network,
    int parent_prefix,
    std::vector<VlsmRequirement> requirements);

} // namespace ccna
