# Lab 01: CIDR Calculator (C++23)

## Goal
Given an IPv4 address and a prefix length (CIDR), compute:
- subnet mask
- network address
- broadcast address
- usable host range

## Starter code
Edit `main.cpp` to parse inputs and output results.

## Input format
Hardcode or ask user for:
- `ip/prefix`

Example: `192.168.10.5/24`

## Expected output (example)
For `192.168.10.5/24`:
- mask: 255.255.255.0
- network: 192.168.10.0
- broadcast: 192.168.10.255
- usable: 192.168.10.1 - 192.168.10.254

