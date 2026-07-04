# Lab 02: Routing Table Longest Prefix Match (LPM)

## Goal
Model a simple routing table and perform **longest-prefix match** lookups.

## Concepts
- A route destination is a prefix: `A.B.C.D/p`
- A route matches a destination IP if the IP lies inside the prefix’s network range.
- Choose the route with the **largest prefix length**.

## Exercise
Use the provided `main.cpp` and verify:
- `10.1.2.55` → `/24` route
- `10.1.88.1` → `/16` route
- `10.22.1.1` → `/8` route

## Build
From this lab folder:
```bash
g++ -std=c++23 -O2 -Wall -Wextra -pedantic ../../src/ip.cpp ../../src/routing.cpp main.cpp -o lpm
./lpm
```

