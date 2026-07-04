# Subnetting Practice (Answer Key)

## Example set
Assume parent network: `192.168.10.0/24`

### (A) Split into 4 equal /26 subnets
- `/26` block size: 64 addresses
- Usable hosts per /26: 62

Subnets:
1. 192.168.10.0/26
2. 192.168.10.64/26
3. 192.168.10.128/26
4. 192.168.10.192/26

### (B) VLSM for host requirements: 120, 60, 30, 10
Compute prefix per requirement:
- 120 hosts → need 128 addresses → `/25` (usable 126)
- 60 hosts → need 64 addresses → `/26` (usable 62)
- 30 hosts → need 32 addresses → `/27` (usable 30)
- 10 hosts → need 16 addresses → `/28` (usable 14)

Allocate largest first starting at `192.168.10.0`:
1. 120 hosts: 192.168.10.0/25 (broadcast 192.168.10.127)
2. 60 hosts: 192.168.10.128/26 (broadcast 192.168.10.191)
3. 30 hosts: 192.168.10.192/27 (broadcast 192.168.10.223)
4. 10 hosts: 192.168.10.224/28 (broadcast 192.168.10.239)

> Note: Expected outputs depend on allocation ordering; this key uses descending size then sequential allocation.

