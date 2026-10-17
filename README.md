# CCNA Programming in C++23

A practical, program-oriented networking study repository using C++23. The exercises model IPv4 and IPv6 addressing, CIDR/subnet calculations, VLSM allocation, routing tables, and longest-prefix-match (LPM) forwarding.

> **Scope:** “IPv7/8/9/10” are not standardized IP protocol versions in the IETF Internet Protocol suite. The repository treats IPv4 and IPv6 as separate protocol/addressing modules rather than inventing unsupported protocol versions.

## Repository map

- `src/` — reusable IPv4, IPv6, VLSM, and routing utilities.
- `docs/` — concept notes and learning guides.
- `labs/` — focused, runnable practice programs.
- `projects/` — larger networking exercises.
- `tests/` — unit tests for address, subnetting, and routing behavior.
- `.github/workflows/` — continuous integration.

## Requirements

- CMake 3.20+
- A C++23 compiler (recent GCC, Clang, or MSVC)
- Git

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run programs after building:

```bash
./build/cidr-calculator
./build/routing-lpm
./build/ipv6-cidr
./build/vlsm-allocator
```

On Windows, executables are typically under `build/Debug` or `build/Release`, depending on the generator.

## Standalone compile examples

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic labs/01-cidr-calculator/main.cpp src/ip.cpp -o cidr-calculator
g++ -std=c++23 -Wall -Wextra -Wpedantic labs/02-routing-lpm/main.cpp src/ip.cpp src/routing.cpp -o routing-lpm
g++ -std=c++23 -Wall -Wextra -Wpedantic labs/03-ipv6-cidr/main.cpp src/ipv6.cpp src/ipv6_routing.cpp -o ipv6-cidr
g++ -std=c++23 -Wall -Wextra -Wpedantic projects/02-vlsm-allocator/main.cpp src/ip.cpp src/vlsm.cpp -o vlsm-allocator
```

## Learning path

1. [Getting started](docs/00-Getting-Started.md)
2. [IPv4, CIDR and VLSM](docs/01-IPv4-CIDR-VLSM.md)
3. [Subnetting practice and answer key](docs/02-Subnetting-Practice-Answer-Key.md)
4. [Routing table model](docs/03-Routing-Tables-Model.md)
5. [Routing graph and packet forwarding](docs/04-Routing-Graph-%26-Packet-Fowards.md)
6. [IPv6 addressing and routing](docs/05-IPv6.md)
7. [Program layout](docs/PROGRAM_LAYOUT.md)
8. [Build, test, and conventions](docs/BUILD-AND-TEST.md)
9. [CIDR calculator lab](labs/01-cidr-calculator/)
10. [Longest-prefix-match lab](labs/02-routing-lpm/)
11. [IPv6 CIDR and LPM lab](labs/03-ipv6-cidr/)
12. [VLSM allocator project](projects/02-vlsm-allocator/)

## Educational disclaimer

These examples are learning tools, not production networking software and not a substitute for current official CCNA exam objectives or hands-on practice with network devices. Verify routing and subnetting behavior against the scenario being studied, especially special prefixes such as IPv4 `/31` and `/32`.
