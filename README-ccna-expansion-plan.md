# CCNA C++23 expansion plan (files + structure)

This repo currently contains a CCNA-style C++23 foundation (CIDR/VLSM/routing LPM).

## What I will create next (high-level)
- Additional docs for CCNA IPv4 topics, subnetting, VLSM, routing tables, LPM walkthroughs.
- Additional code modules in `src/` (utility helpers, parsers, routing graph model, packet forward simulator stubs).
- Additional labs/projects:
  - CIDR calculator expansions
  - VLSM allocator expansions
  - Routing table model + LPM routing lab
  - Routing graph + packet forwarding lab
- Optional data-layer stubs:
  - SQL schema + example data export to CSV/JSON
- Design docs (UML-style text diagrams / Mermaid).

## Required scope (needed before generating everything)
Because your request lists many variants (IPv7/8/9/10, bit-width variants including 8/16/32/64/96/128/256, SQL + Excel formats, UML docs, algorithms/equations/formulas), I need a concrete target scope to avoid generating the wrong or massive content.

## Proposed minimal scope (recommended)
1. Keep focus on IPv4 CCNA content + coding exercises (what the repo already demonstrates).
2. Add documentation for IPv6 at a later stage (if desired), not IPv7.
3. Implement bit-width variants only as generic utilities (e.g., templated address size) but only fully document + test for 32-bit (IPv4).

## Output checklist
When complete, this repo will include:
- `docs/` expanded with sectioned handouts
- `src/` expanded modules
- `labs/` expanded with runnable C++23 code
- `projects/` optional runnable demos
- `database/` with SQL schema and sample exports (if enabled)
- `design/` with UML/architecture docs


