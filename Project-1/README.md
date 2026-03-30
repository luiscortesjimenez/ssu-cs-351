# Project 1 — Remote GitHub Development and Performance Monitoring

## Experiment Overview

Four implementations of the same algorithm (building a linked list and hashing its data) were benchmarked under various configurations:

| Program | Allocation Method |
|---|---|
| `alloca.cpp` | `alloca()` — allocates on the stack via recursion |
| `malloc.cpp` | `malloc()` + placement `new` — allocates on the heap (C-style) |
| `new.cpp` | `operator new` — allocates on the heap (C++-style), manual linked list |
| `list.cpp` | `std::list` + `std::vector` — allocates on the heap (C++ STL) |

---

## Experiment A — Compiler Optimization

### Default (`-g`)

| Program | Min | Avg | Max |
|---|---|---|---|
| alloca | 0.000 | 0.009 | 0.010 |
| list | 0.010 | 0.019 | 0.020 |
| malloc | 0.000 | 0.009 | 0.010 |
| new | 0.010 | 0.019 | 0.020 |

### Optimized (`-O2 -g2`)

| Program | Min | Avg | Max |
|---|---|---|---|
| alloca | 0.000 | 0.000 | 0.000 |
| list | 0.000 | 0.003 | 0.010 |
| malloc | 0.000 | 0.000 | 0.000 |
| new | 0.000 | 0.003 | 0.010 |

---

## Experiment B — Data Per Node Size

All runs use `OPT="-O2 -g2"`, `NUM_BLOCKS=10000`.

### Small (MIN_BYTES=10, MAX_BYTES=10)

| Program | Min | Avg | Max |
|---|---|---|---|
| alloca | 0.000 | 0.000 | 0.000 |
| list | 0.000 | 0.000 | 0.000 |
| malloc | 0.000 | 0.000 | 0.000 |
| new | 0.000 | 0.000 | 0.000 |

### Medium (MIN_BYTES=100, MAX_BYTES=500)

| Program | Min | Avg | Max |
|---|---|---|---|
| alloca | 0.000 | 0.009 | 0.010 |
| list | 0.000 | 0.009 | 0.010 |
| malloc | 0.010 | 0.010 | 0.010 |
| new | 0.000 | 0.009 | 0.010 |

### Large (MIN_BYTES=1000, MAX_BYTES=4000)

| Program | Min | Avg | Max |
|---|---|---|---|
| alloca | 0.050 | 0.058 | 0.060 |
| list | 0.050 | 0.057 | 0.060 |
| malloc | 0.050 | 0.058 | 0.060 |
| new | 0.050 | 0.058 | 0.060 |

### Very Large (MIN_BYTES=4096, MAX_BYTES=8192)

| Program | Min | Avg | Max |
|---|---|---|---|
| alloca | 0.130 | 0.143 | 0.160 |
| list | 0.150 | 0.159 | 0.160 |
| malloc | 0.130 | 0.139 | 0.150 |
| new | 0.130 | 0.139 | 0.150 |

---

## Experiment C — Block Chain Length

All runs use `OPT="-O2 -g2"`, default `MIN_BYTES=3`, `MAX_BYTES=100`.

| NUM_BLOCKS | alloca (avg) | malloc (avg) | new (avg) | list (avg) |
|---|---|---|---|---|
| 10,000 | 0.000 | 0.000 | 0.004 | 0.004 |
| 100,000 | 0.019 | 0.020 | 0.027 | 0.029 |
| 1,000,000 | 0.145 | 0.167 | 0.208 | 0.218 |
| 10,000,000 | 1.383 | 1.546 | 2.010 | 2.066 |

---

## Experiment D — Heap Breaks

All runs use default compilation (`-g`), default `MIN_BYTES=3`, `MAX_BYTES=100`.

| NUM_BLOCKS | alloca | malloc | list | new |
|---|---|---|---|---|
| 10,000 | 69 | 76 | 78 | 78 |
| 100,000 | 69 | 137 | 156 | 156 |
| 1,000,000 | 69 | 751 | 933 | 933 |

---

## Analysis

### 1. Which program is fastest? Is it always the fastest?

alloca is the fastest overall. In Experiment C at 10 million blocks it averaged 1.383s and beat malloc at 1.546s, new at 2.010s, and list at 2.066s. Its not always the fastest, in experiment B with very large node data (MIN_BYTES=4096, MAX_BYTES=8192), malloc and new tied it or edged it out slightly (0.139 avg vs 0.143). When the data per node gets big enough the hashing work is much better and the allocation advantage of alloca matters less.

### 2. Which program is slowest? Is it always the slowest?

list is generally the slowest. At 10 million blocks it averaged 2.066s compared to everything else. It's not always the slowest, for example new is very close to it in most tests (2.010 vs 2.066 at 10M blocks) and in some of the smaller or large data tests they're basically tied. Both list and new are consistently slower than alloca and malloc because they have higher per node allocation overhead from C++.

### 3. Was there a trend in program execution time based on the size of data in each Node? If so, what, and why?

Yes, as the data size per node increased all programs got slower and their runtimes approached the same time. At small data sizes everything ran at 0.000s. At very large data sizes all four programs were between 0.139–0.159s. This makes sense because with bigger data the time spent initializing and hashing the bytes dominates the total runtime. The allocation method becomes less significant since all programs do the same initialization and hashing work.

### 4. Was there a trend in program execution time based on the length of the block chain?

Yes, runtime scaled pretty much linearly with chain length. Going from 10k to 100k to 1M to 10M blocks the times increased by roughly 10x each step. For example alloca went from 0.000 to 0.019 to 0.145 to 1.383. The gap between the faster programs (alloca and malloc) and the slower ones (new and list) also got wider as the chain got longer since the per node allocation overhead adds up over millions of iterations.

### 5. Consider heap breaks, what's noticeable? Does increasing the stack size affect the heap? Speculate on any similarities and differences in programs.

The most noticeable thing is that alloca stays at exactly 69 breaks no matter how many blocks are created (10k, 100k, or 1M). This makes sense because alloca allocates on the stack not the heap so it never needs to request more heap memory beyond the programs initial setup.
The other three programs all increase their break counts as NUM_BLOCKS grows. list and new are identical (78, 156, 933) which suggests they have similar heap allocation patterns since both allocate individual nodes on the heap. malloc uses fewer breaks (76, 137, 751), probably because its raw malloc calls have slightly less overhead per allocation than operator new or the std::list internal allocator.
Increasing the stack size with ulimit -s unlimited doesnt affect the heap directly, it just allows alloca to use more stack space without crashing. The heap and stack grow in opposite directions in memory so they dont interfere with each other unless the process runs out of virtual address space.

### 6. Node Diagram

Considering `malloc.cpp`, below is a diagram of two Nodes each with 6 bytes of data. Each node is allocated as one contiguous block via `malloc(sizeof(Node) + numBytes)`.

**Node struct fields:** `next` (8 bytes), `numBytes` (4 bytes), `bytes` (8 bytes, points into data region after struct)

```
 head ----+      +-------------------------------------------+     tail
          |      |                                           |       |
          v      |                                           v       v
        +--------+------+---------+--+--+--+--+--+--+     +--------+------+---------+--+--+--+--+--+--+
Node 1: | next   | nB=6 | bytes ---->| 1| 2| 3| 4| 5| 6|  | next   | nB=6 | bytes ---->| 1| 2| 3| 4| 5| 6|
        | (8B)   | (4B) | (8B)   |  (6 bytes of data) |  | nullptr| (4B) | (8B)   |  (6 bytes of data) |
        +--------+------+---------+--------------------+  +--------+------+---------+--------------------+
        |<------- sizeof(Node) = 20B ------>|<-- 6B -->|  |<------- sizeof(Node) = 20B ------>|<-- 6B -->|
        |<---------- malloc'd: 26 bytes ----------->|     |<---------- malloc'd: 26 bytes ----------->|

Node 1's next --> Node 2
Node 2's next = nullptr
```

### 7. There's an overhead to allocating memory, initializing it, and eventually processing (hashing) it. For each program, were any of these tasks the same? Which one(s) were different?

Initialization and hashing are the same across all four programs. They all use std::iota to fill the data bytes with values 1, 2, 3 and so on and the hash function is identical in every version.
The allocation is different. list uses std::list to manage nodes and std::vector for byte storage so allocation is handled entirely by the STL. new uses operator new to allocate each Node on the heap and the data bytes live in a std::vector inside the Node. malloc uses malloc() to get a raw block of memory then placement new to construct the Node in that memory, with the data bytes stored inline right after the struct fields. alloca is basically the same as malloc but uses alloca() to allocate on the stack instead of the heap and uses recursion to keep each stack frame alive.

### 8. As the size of data in a Node increases, does the significance of allocating the node increase or decrease?

It decreases when nodes are small the time spent allocating memory is a bigger chunk of the total work so the allocation method matters more. Thats why we see clear differences between programs at small data sizes with many blocks. But as the data per node grows the time spent initializing and hashing the bytes takes over. In Experiment B at very large sizes all four programs converged to nearly the same runtime (0.139 to 0.159s) showing that allocation overhead became insignificant compared to the data processing work.