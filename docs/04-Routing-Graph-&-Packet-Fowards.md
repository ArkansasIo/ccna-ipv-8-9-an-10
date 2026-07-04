# Packet forwarding simulation (simple lab)

## Goal
Model how a router uses a routing table to forward packets.

## CCNA concept mapping
- Router receives destination IP
- Performs **routing table lookup (LPM)**
- Sends out to next hop

## Programming mapping
Implement a tiny simulator:
1. Parse a list of routes (prefix → next hop)
2. For each incoming packet destination:
   - run LPM lookup
   - output which next hop would be used

## Suggested exercises
- Extend Lab 02 by adding:
  - 1 default route: `0.0.0.0/0`
  - show that any destination not matching specific routes uses default

## Expected behavior
- For `10.1.2.55` → /24 route
- For `8.8.8.8` → default route

