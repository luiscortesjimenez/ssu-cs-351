# ssu-cs-351
Coursework for Computer Architecture at Sonoma State University.

---

# Project 1 — Remote GitHub Development and Performance Monitoring

## Experiment Overview

Four implementations of the same algorithm (building a linked list and hashing its data) were benchmarked under various configurations:

| Program | Allocation Method |
|---|---|
| `alloca.cpp` | `alloca()` — allocates on the stack via recursion |
| `malloc.cpp` | `malloc()` + placement `new` — allocates on the heap (C-style) |
| `new.cpp` | `operator new` — allocates on the heap (C++-style), manual linked list |
| `list.cpp` | `std::list` + `std::vector` — allocates on the heap (C++ STL) |
