#include "../../src/ip.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace ccna;

struct Requirement {
  int hosts;
  std::string name;
};

static int prefix_for_hosts(int hosts) {
  // Classic IPv4: usable = 2^(32-p) - 2
  // Find smallest block where usable >= hosts.
  if (hosts <= 0) throw std::invalid_argument("hosts must be > 0");
  for (int p = 32; p >= 0; --p) {
    std::uint64_t block = (p == 32) ? 1ull : (1ull << (32 - p));
    if (block < 2) continue;
    std::uint64_t usable = block - 2;
    if (usable >= static_cast<std::uint64_t>(hosts)) return p;
  }
  return 0;
}

struct Alloc {
  std::string name;
  Requirement req;
  int prefix;
  Cidr cidr;
  HostRange usable;
};

static std::vector<Alloc> allocate_vlsm(std::uint32_t parent_network, int parent_prefix,
                                        std::vector<Requirement> reqs) {
  Cidr parent = make_cidr(parent_network, parent_prefix);

  // Determine per requirement prefix.
  struct Work {
    Requirement r;
    int prefix;
  };

  std::vector<Work> work;
  work.reserve(reqs.size());
  for (auto& r : reqs) {
    int p = prefix_for_hosts(r.hosts);
    work.push_back(Work{r, p});
  }

  // Sort by descending prefix length? Actually larger blocks = smaller prefix number.
  // So sort by ascending prefix len (e.g., /25 before /26?). For correctness: allocate largest blocks first.
  // Largest block => smallest prefix_len.
  std::sort(work.begin(), work.end(), [](const Work& a, const Work& b) {
    return a.prefix < b.prefix;
  });

  std::uint32_t cursor = parent.network;
  std::vector<Alloc> out;
  out.reserve(reqs.size());

  for (auto& w : work) {
    int p = w.prefix;
    auto block_mask = mask_from_prefix(p);
    std::uint32_t block_size = (p == 32) ? 1u : (1u << (32 - p));

    // Align cursor to block boundary (within parent).
    // For a given prefix, block boundary means cursor % block_size == 0 relative to parent base.
    std::uint32_t offset = cursor - parent.network;
    std::uint32_t rem = offset % block_size;
    if (rem != 0) cursor += (block_size - rem);

    // Check fit within parent.
    if (cursor + block_size - 1 > parent.broadcast) {
      throw std::runtime_error("Not enough space in parent network for VLSM allocation");
    }

    auto cidr = make_cidr(cursor, p);
    auto usable = usable_host_range(cidr);
    out.push_back(Alloc{w.r.name, w.r, p, cidr, usable});

    cursor = cidr.broadcast + 1;
  }

  return out;
}

int main() {
  std::string parent_str = "192.168.10.0/24";
  std::uint32_t parent_ip = 0;
  int parent_prefix = 0;

  // parse parent
  auto slash = parent_str.find('/');
  auto ip_str = parent_str.substr(0, slash);
  auto p_str = parent_str.substr(slash + 1);
  parent_prefix = std::stoi(p_str);
  auto parsed = parse_ipv4(ip_str);
  if (!parsed) {
    std::cerr << "Invalid parent";
    return 1;
  }
  parent_ip = *parsed;

  std::vector<Requirement> reqs = {
      {120, "net-A"},
      {60, "net-B"},
      {30, "net-C"},
      {10, "net-D"},
  };

  try {
    auto allocs = allocate_vlsm(parent_ip, parent_prefix, reqs);

    for (auto& a : allocs) {
      std::cout << a.name << ": "
                << format_ipv4(a.cidr.network) << "/" << a.cidr.prefix_len
                << "  broadcast=" << format_ipv4(a.cidr.broadcast);

      if (a.usable.first && a.usable.last) {
        std::cout << "  usable=" << format_ipv4(*a.usable.first) << "-" << format_ipv4(*a.usable.last);
      }
      std::cout << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Allocation error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

