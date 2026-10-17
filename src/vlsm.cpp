#include "vlsm.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace ccna {

int prefix_for_hosts(int hosts) {
  if (hosts <= 0) {
    throw std::invalid_argument("hosts must be greater than zero");
  }

  // Traditional subnetting excludes network and broadcast addresses.
  // A /31 therefore provides zero traditional usable hosts.
  for (int prefix = 30; prefix >= 0; --prefix) {
    const std::uint64_t block_size = 1ULL << (32 - prefix);
    const std::uint64_t usable = block_size - 2ULL;
    if (usable >= static_cast<std::uint64_t>(hosts)) {
      return prefix;
    }
  }

  throw std::invalid_argument("host requirement is too large for an IPv4 subnet");
}

std::vector<VlsmAllocation> allocate_vlsm(
    std::uint32_t parent_network,
    int parent_prefix,
    std::vector<VlsmRequirement> requirements) {
  if (requirements.empty()) {
    return {};
  }

  const Cidr parent = make_cidr(parent_network, parent_prefix);

  struct WorkItem {
    VlsmRequirement requirement;
    int prefix;
  };

  std::vector<WorkItem> work;
  work.reserve(requirements.size());
  for (const auto& requirement : requirements) {
    work.push_back(WorkItem{requirement, prefix_for_hosts(requirement.hosts)});
  }

  // Largest block first is the standard VLSM allocation strategy.
  std::stable_sort(work.begin(), work.end(), [](const WorkItem& a, const WorkItem& b) {
    return a.prefix < b.prefix;
  });

  std::uint64_t cursor = parent.network;
  const std::uint64_t parent_end = parent.broadcast;

  std::vector<VlsmAllocation> allocations;
  allocations.reserve(work.size());

  for (const auto& item : work) {
    const std::uint64_t block_size = 1ULL << (32 - item.prefix);
    const std::uint64_t remainder = cursor % block_size;
    if (remainder != 0) {
      cursor += block_size - remainder;
    }

    if (cursor > parent_end || block_size - 1 > parent_end - cursor) {
      throw std::runtime_error("not enough space in parent network for VLSM allocation");
    }

    const auto network = static_cast<std::uint32_t>(cursor);
    const Cidr cidr = make_cidr(network, item.prefix);
    allocations.push_back(VlsmAllocation{
        item.requirement.name,
        item.requirement.hosts,
        item.prefix,
        cidr,
        usable_host_range(cidr)});

    cursor += block_size;
  }

  return allocations;
}

} // namespace ccna
