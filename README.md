# Cerbero

A high-performance, real-time IPv4 filtering and blacklist engine written in C11, powered by dynamic **van Emde Boas (vEB)** trees and **SipHash-2-4** protected hash tables.

Designed for high-throughput network security and data-plane firewalls, Cerbero provides sub-microsecond $O(\log \log U)$ lookups over the entire 32-bit IPv4 address space ($U = 2^{32}$) with dynamic memory allocation and HashDoS resilience.

---

## Current Status

- [x] **van Emde Boas (vEB) Tree**: Core mathematical engine implemented with dynamic fractal bit-partitioning for the entire 32-bit IPv4 space ($U = 2^{32}$).
- [x] **Internal Hash Tables**: Open-addressing (*linear probing*) dynamic hash tables implemented for sparse child cluster storage.
- [x] **SipHash-2-4**: Cryptographic PRF implemented using a 128-bit key (`getrandom()`) to defend against HashDoS collision attacks.
- [x] **CerberoEngine**: Unified abstraction implemented to encapsulate root vEB tree and master key lifecycle (`cerbero_engine_create`, `cerbero_destroy`).
- [x] **Memory Integrity**: Verified clean cascading cleanup under Valgrind with 0 memory leaks and 0 pointer errors.
- [x] **High Throughput**: Search operations validated at sub-microsecond latency, capable of exceeding >1.3M+ lookups per second on a single thread.

---

## Key Features

* **$O(\log \log U)$ Lookup Complexity**: Worst-case search and insertion times scale with $\log_2(\log_2(2^{32})) \le 5$ recursive bit-decomposition steps, providing consistent sub-microsecond latency regardless of table size.
* **HashDoS-Resistant Cluster Storage**: Internal cluster nodes use open-addressing (*linear probing*) hash tables keyed with SipHash-2-4 using a cryptographically secure 128-bit master key (`getrandom()`).
* **Sparse / Dynamic Memory Layout**: Clusters are allocated on-demand; empty IP address blocks consume zero memory.
* **Zero Memory Leaks**: Clean cascading destruction (`cerbero_destroy`, `veb_destroy`, `hashtable_destroy`) verified under Valgrind with 0 errors and 0 leaked bytes.
* **High Throughput**: Sustained search operations exceed **1.3M+ operations per second (OPS)** on a single CPU core.

---

## Architectural Overview

```
                        +----------------------------+
                        |       CerberoEngine        |
                        |  - secret_key[16] (128b)   |
                        |  - tree_root (vEB capacity)|
                        +--------------+-------------+
                                       |
                   +-------------------+-------------------+
                   | vEB Tree (U = 2^32, capacity = 32)    |
                   |   min, max, is_empty                  |
                   +---------+-------------------+---------+
                             |                   |
                     [top / summary]      [bottom / clusters]
                     (recursive vEB)      (HashTable via SipHash)
```

1. **Fractal Division ($U \to \sqrt{U}$)**:
   Any 32-bit IPv4 address $x$ is divided into upper bits `high = x >> (capacity / 2)` and lower bits `low = x & ((1 << (capacity / 2)) - 1)`.
2. **Top / Summary**:
   Maintains which cluster buckets in the lower half currently contain at least one element.
3. **Bottom / Clusters**:
   Dynamic hash tables index active child clusters using SipHash-2-4 to prevent hash collision attacks.

---

## Project Structure

```
.
├── include/
│   ├── cerbero.h      # CerberoEngine lifecycle and API declarations
│   ├── veb.h          # van Emde Boas tree core interface
│   ├── hashtable.h    # Open-addressing hash table interface
│   └── siphash.h      # SipHash-2-4 PRF implementation
├── src/
│   ├── cerbero.c      # CerberoEngine implementation
│   ├── veb.c          # vEB insert, contains, create, destroy
│   ├── hashtable.c    # Hash table operations and dynamic resizing
│   └── siphash.c      # SipHash-2-4 round transformations
├── Makefile           # Build automation
└── README.md          # Project documentation
```

---

## Building and Running

> [!NOTE]
> Cerbero is currently structured as an engine library. A standalone `main` entrypoint executable has not yet been included in the repository.

### Requirements
* GCC with C11 support (`-std=c11`)
* Linux kernel with `getrandom()` support
* Make
* Valgrind (optional, for memory profiling)

---

## What's Next (Roadmap)

- **`cerbero_engine_start` Lifecycle API & Configuration**: Introduce a decoupled `CerberoConfig` structure (network interface binding, worker thread count, ring buffer sizing, custom callbacks) and an engine entry point `cerbero_engine_start(CerberoEngine *engine, const CerberoConfig *config)` to enable Cerbero as an embeddable library.
- **Zero-Copy / Syscall Kernel Bypass Packet Scanning**: Native integration of shared-memory ring buffers (`PACKET_MMAP` / AF_XDP) directly into the engine core, eliminating `recvfrom` syscalls and memory copies for line-rate packet ingestion.
- **Multi-Threading via Kernel `PACKET_FANOUT`**: Distribute network traffic across CPU cores using socket fanout (`PACKET_FANOUT_HASH` / `PACKET_FANOUT_CPU`), where each worker thread maintains a dedicated zero-copy ring buffer to scale throughput linearly without lock contention.
- **Read-Write Locking (RWLock) for Dynamic Rule Updates**: Implement a thread-safe Reader-Writer Lock around the van Emde Boas tree, allowing concurrent zero-overhead read searches (`veb_contains`) while safely supporting live dynamic IP blacklist insertions and removals in real time without pausing traffic.
- **Control Plane Socket for Dynamic Blacklist Updates**: Introduce a dedicated control-plane socket (e.g., Unix Domain Socket) to receive new IPs and CIDRs dynamically from external monitoring tools, orchestrators, or administrators at runtime without restarting the engine.
- **Performance Metrics & Statistics Export**: Provide mechanisms to export operational statistics and performance metrics (e.g., packet throughput, drop rates, and bandwidth) for monitoring and post-run analysis.

---

## References

- [SipHash: a fast short-input PRF - Aumasson & Bernstein](https://eprint.iacr.org/2012/351.pdf)
- [Divide & Conquer: van Emde Boas Trees - MIT](https://www.youtube.com/watch?v=hmReJCupbNU&t=1278s)
- [Apunte Diseño y Análisis de Algoritmos - Gonzalo Navarro](https://users.dcc.uchile.cl/~gnavarro/apunte2.pdf)

