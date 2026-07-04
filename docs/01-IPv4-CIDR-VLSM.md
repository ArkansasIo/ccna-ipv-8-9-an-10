# IPv4 CIDR & VLSM (How to implement it in C++)

## 1) Core definitions
- **IPv4 address**: 32-bit value (0.0.0.0 to 255.255.255.255)
- **CIDR**: network prefix length `/p` where `0 <= p <= 32`
- **Subnet mask**: derived from prefix length
- **Network address**: base address where host bits are 0
- **Broadcast address**: base address where host bits are 1

## 2) Mapping to programming
Represent IPv4 as a `uint32_t`.
- Convert dotted-decimal string → 32-bit integer
- Convert prefix length `/p` → mask
- Compute:
  - network = ip & mask
  - broadcast = network | ~mask
  - number of addresses = 2^(32-p)

## 3) VLSM algorithm (common approach)
Given a set of required host counts:
1. For each requirement, compute needed prefix `p` such that:
   - usable_hosts = 2^(32-p) - 2
   - usable_hosts >= required_hosts
2. Sort by descending prefix (largest subnets first).
3. Allocate subnets sequentially from the parent network base, aligned on mask boundaries.

## 4) Expected edge cases
- Host counts may be too large for the chosen parent block
- /32 means 1 address (no usable hosts conventionally)
- Validate input format (e.g. `300.1.1.1` is invalid)

## 5) Suggested exercises
1. Given `192.168.10.0/24`, compute:
   - subnet mask
   - network and broadcast
2. Allocate VLSM subnets from a /24 for host requirements:
   - 120, 60, 30, 10
3. Output each subnet:
   - CIDR, usable range, broadcast

