# Project 02: VLSM Allocator (C++23)

## Goal
Allocate VLSM subnets from a parent CIDR for a list of host requirements.

## Output
For each subnet, print:
- subnet CIDR
- broadcast
- usable range (classic: network and broadcast excluded)

## Build (GCC/Clang example)
From this folder:
```bash
g++ -std=c++23 -O2 -Wall -Wextra -pedantic ../../src/ip.cpp main.cpp -o vlsm
./vlsm
```

## Notes
This is a model allocator for learning the subnet math.

