# CCNA Repo Program Layout (Guide)

This guide explains the repo structure and how to run the included labs/projects.

## 1) Top-level folders

- **`docs/`**
  - CCNA concepts translated into programming exercises (theory + lab handouts).
  - Example docs: `00-Getting-Started.md`, `01-IPv4-CIDR-VLSM.md`, `03-Routing-Tables-Model.md`.

- **`src/`**
  - Reusable C++23 “library” code used by labs.
  - Current modules include:
    - `ip.hpp` / `ip.cpp` (IPv4 parsing + CIDR/CIDR mask math)
    - `routing.hpp` / `routing.cpp` (routing table models, LPM lookup, etc.)

- **`labs/`**
  - Complete, runnable exercises.
  - Each lab typically contains:
    - `README.md` (goal + expected behavior)
    - `main.cpp` (starter code)

- **`projects/`**
  - Larger optional implementations (bigger scope than labs).

## 2) Running labs

### Lab 01: CIDR Calculator
Location: `labs/01-cidr-calculator/`

From the lab folder, build with a simple compile command:

```bat
g++ -std=c++23 -O2 -Wall -Wextra -pedantic ..\..\src\ip.cpp main.cpp -o cidr
```

Run:

```bat
.idr
```

> If your code also uses routing helpers, add `..\..\src\routing.cpp` to the compile command.

### Lab 02: Routing Table Longest Prefix Match (LPM)
Location: `labs/02-routing-lpm/`

```bat
g++ -std=c++23 -O2 -Wall -Wextra -pedantic ..\..\src\ip.cpp ..\..\src\routing.cpp main.cpp -o lpm
```

Run:

```bat
.lpm
```

## 3) Code dependencies (how labs use `src/`)

Typical pattern:

- `main.cpp` includes library headers:
  - `#include "../../src/ip.hpp"`
  - `#include "../../src/routing.hpp"`

- The build links the corresponding `.cpp` files in `src/`.

## 4) CIDR / prefix constraints (important)

The current implementation is **IPv4-only**.

- Prefix length **`/p` is enforced for `0 <= p <= 32`**.
- Prefixes like `/64`, `/96`, `/128`, `/256` are **not supported** by the current 32-bit IPv4 representation.

When the repo expands toward bigger address sizes (e.g., 128-bit IPv6-like math), this guide should be updated with the new modules + address representation.

