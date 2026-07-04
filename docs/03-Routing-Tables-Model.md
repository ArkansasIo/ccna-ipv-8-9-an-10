# Routing Tables (CCNA concepts) — model them in C++

## Goal
Understand how routing tables conceptually store:
- destination networks (prefix)
- next-hop / outgoing interface
- administrative distance / metrics
- longest-prefix match

## Mapping to code
### 1) Prefix + next-hop
A route is usually represented as:
- `prefix` (e.g. `10.0.0.0/8`)
- `next_hop` (e.g. `192.168.1.1`) or outgoing interface
- `prefix_len`
- optional metric/AD

### 2) Lookup: Longest Prefix Match (LPM)
Given a destination IP:
1. Compute which route prefixes match (destination & mask == network)
2. Choose the route with the **largest prefix length**

## Suggested exercises
1. Create a route list in code with 3 routes:
   - `10.0.0.0/8` → next hop A
   - `10.1.0.0/16` → next hop B
   - `10.1.2.0/24` → next hop C
2. Lookup destination `10.1.2.55` → should pick `/24`
3. Lookup destination `10.1.88.1` → should pick `/16`
4. Lookup destination `10.22.1.1` → should pick `/8`

## Expected outputs
Show chosen next hop for each lookup.

## Build hint
You can build on `src/ip.hpp` for CIDR math.

