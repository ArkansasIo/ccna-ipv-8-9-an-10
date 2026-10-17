# Build, test, and networking conventions

## Project conventions

- Core library code belongs in `src/`.
- Small guided exercises belong in `labs/`.
- Multi-step applications belong in `projects/`.
- Unit tests belong in `tests/`.
- Build behavior is defined in the root `CMakeLists.txt`; CI verifies it on Linux, Windows, and macOS.

## IPv4 address representation

IPv4 values are represented as unsigned 32-bit integers in network-order significance: the first octet occupies the most significant 8 bits. The range is exactly 0 through 2^32-1.

For a valid prefix length p in [0, 32], the subnet mask is:

`mask(p) = 0` when p = 0; otherwise `0xffffffff << (32-p)`.

Network and broadcast addresses are calculated using the aligned address:

- `network = address AND mask`
- `broadcast = network OR (NOT mask)`

The current `usable_host_range` function uses the classic teaching convention that excludes network and broadcast addresses; it returns no host range for /31 and /32. In real networking, /31 point-to-point links are commonly interpreted under RFC 3021, so lab answers should state which convention they use.

## Longest prefix match

For each route, the destination matches if it lies between the route's network and broadcast address. The selected route is the matching route with the greatest prefix length. A default route is represented by `0.0.0.0/0`.

## VLSM allocation

Allocate the largest requested subnets first. Calculate each subnet prefix from the host requirement, align each subnet start to its block-size boundary, and verify that the final address remains inside the parent network. In a production implementation, perform bounds checks with a wider integer type to avoid 32-bit overflow at the top of the address space.

## IPv6 boundary

IPv6 uses 128-bit addresses and is not a larger IPv4 prefix. Do not reuse IPv4's `uint32_t` mask or arithmetic for IPv6. A future IPv6 module should use a dedicated 128-bit representation.
