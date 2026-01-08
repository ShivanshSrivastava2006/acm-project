# Memory Management Simulator -- Design Document

Date: 2026-01-07  
Author: ShivanshSrivastava2006 (project repository)

## Overview

This document concisely describes the design and rationale of the Memory Management Simulator implemented in C++17 for the OS / ACM-style assignment. The simulator models a contiguous, metadata-only, byte-addressable physical memory and provides two allocation subsystems: a family of general-purpose allocators (First Fit, Best Fit, Worst Fit) that share a common allocator interface, and a separate Buddy allocator subsystem that enforces power-of-two sizing. A multilevel cache simulator (L1 + L2) and an interactive CLI/REPL complete the user-facing functionality.

The implementation is complete and the design description below explains the structure, invariants, algorithms, and trade-offs that guided the implementation.

## High-Level Architecture

- Language: Modern C++ (C++17)
- Process memory: simulated, metadata-only; no real memory contents are stored.
- Top-level components:
  - Core memory model (address space and metadata structures)
  - Allocator interface and three allocation strategies (FF/BF/WF)
  - Buddy allocator subsystem
  - Multilevel cache simulator (L1 and L2)
  - CLI / REPL that orchestrates initialization, commands, and statistics
- Build: single Makefile producing an executable (no external dependencies)

Logical component diagram (textual):

- main
  - CLI/REPL
    - MemoryController (owns the physical memory metadata)
      - Allocator (polymorphic: FirstFit/BestFit/WorstFit)
      - BuddyAllocator (separate, selected explicitly)
    - CacheController (manages L1 and L2 caches)
    - StatisticsManager (aggregates memory & cache stats)

Key invariants:
- The physical address space is contiguous and byte-addressable.
- Allocation/deallocation operations operate only on metadata (start address, size, id).
- Block IDs are unique and used to identify allocations for deallocation and reporting.
- Allocator selection is runtime-switchable for the general-purpose allocator family; the Buddy allocator is a distinct mode that has its own preconditions.

## Core Memory Simulator

Purpose: Provide a stable, minimal abstraction of contiguous physical memory that supports allocation and deallocation operations performed by the allocator subsystems.

Primary data model:
- The memory is represented by its total size (size_t) and two metadata tables:
  - Allocated blocks: keyed by block ID, value contains {start_address, size}.
  - Free blocks: a data structure maintained by allocators (see allocator sections) that tracks free ranges {start_address, size}.
- Addresses are modeled as 0..(total_size - 1) using size_t. All address arithmetic is bounds-checked.

Invariants and safety checks:
- Allocations must fit entirely within [0, total_size).
- Allocator interfaces ensure no overlapping allocations by updating free and allocated metadata atomically (single-threaded environment).
- On deallocation, free blocks are coalesced (where applicable) to reduce external fragmentation for the FF/BF/WF allocators; the Buddy allocator performs merges according to buddy invariants (see Buddy section).

Memory statistics:
- Used bytes = sum of sizes in allocated blocks.
- Free bytes = total_size − used bytes.
- Utilization % = used_bytes / total_size * 100.
- External fragmentation % = 1 − (size_of_largest_free_block / free_bytes) when free_bytes > 0, else 0. This metric expresses how the free space is fragmented relative to the largest contiguous free range.

Design rationale:
- Metadata-only simulation simplifies reasoning about allocation algorithms and statistics while still accurately reflecting fragmentation behavior and allocation patterns.
- The simulator exposes and records addresses and sizes to make behavior and invariants visible for assignment grading.

## Allocation Strategy Design (First Fit / Best Fit / Worst Fit)

Overview:
- The three allocation strategies implement a shared Allocator interface that the MemoryController uses polymorphically. The interface provides:
  - allocate(request_size) -> either block_id + start_address or failure
  - deallocate(block_id) -> success/failure
  - query/free-list introspection for dumps and statistics

Internal data structures:
- Free blocks are stored in a list-like container. For simplicity and clarity of reasoning, a doubly-linked list (or std::list) of free segments is used together with an ordered index by address to support coalescing.
- Allocated blocks are tracked in an associative map keyed by block ID.

Per-strategy behavior:
- First Fit:
  - Walk the free list from low address to high address; pick the first block whose size >= request_size.
  - If the chosen block is strictly larger than request_size, it is split: lower-address portion is allocated and upper portion remains in the free list with adjusted start and size.
- Best Fit:
  - Walk all free blocks and choose the smallest block with size >= request_size (minimizes leftover in chosen block).
  - Splitting behavior same as above.
- Worst Fit:
  - Choose the free block with greatest size, provided it is >= request_size.
  - Splitting behavior same as above.

Block splitting and coalescing:
- Splitting: when the chosen free block is larger than requested size, the free block is reduced and a new allocated block item is inserted. Start addresses are preserved so newly allocated blocks occupy the lower subrange of the original free block (consistent deterministic policy).
- Coalescing: on deallocation, the freed segment is inserted back into the free list at its address-sorted position and adjacent free segments are merged (left and right) to form a larger contiguous free block. Coalescing ensures the free list remains minimal in number of segments.

Complexity and trade-offs:
- These strategies emphasize clarity and portability rather than asymptotically optimal run-time:
  - First Fit: O(n) in worst-case free-list scanning.
  - Best/Worst Fit: O(n) scanning to select candidate.
- Rationale: these are standard allocator strategies pedagogically important for exposing fragmentation and allocation patterns. The common interface enables runtime switching without reinitializing the memory (other than constraints such as free-list reorganization).

Allocator interface (illustrative snippet):
```text
class Allocator {
public:
  // Returns a positive block ID on success, or 0 / negative on failure.
  virtual int allocate(size_t size) = 0;
  virtual bool deallocate(int block_id) = 0;
  virtual std::vector<FreeSegment> freeList() const = 0;
  virtual ~Allocator() = default;
};
```

(Only illustrative; not a verbatim reproduction of the codebase.)

## Buddy Allocator Design

Overview:
- Implemented as a separate subsystem with its own lifecycle and preconditions.
- The total simulated memory size required by the Buddy allocator is constrained to be a power of two; allocation sizes are rounded up to the next power of two.

Data structures:
- Orders: memory is partitioned into orders 0..max_order where order k corresponds to blocks of size 2^k (base unit depends on implementation but is interpreted in bytes).
- Free lists: a vector (indexed by order) of free lists (each list contains start addresses for free blocks at that order).
- Allocated block table: maps block IDs to {start_address, order}.

Allocation algorithm:
1. Round requested size up to the minimal order k such that block_size( k ) >= requested_size.
2. Search for the smallest order j >= k with a non-empty free list.
3. If j == k: remove a block from free_list[j] and assign it to the requester.
4. If j > k: remove a block from free_list[j], repeatedly split into two buddies of order j−1 until order k is reached; each split produces one half returned to free_list[j−1] and one half possibly for further splitting or allocation.
5. Return the start address and record allocation in the allocated table.

Deallocation and merging:
- On free, the block's start address and order are looked up.
- The algorithm computes the buddy address using XOR: buddy_address = start_address ^ block_size(order). Because addresses and block sizes are powers of two, xor yields the correct buddy start address.
- If the buddy is present in the free_list for the same order, the buddy is removed from that free_list and the two blocks are merged into a block of order+1 whose start address is the min(start_address, buddy_address). This process recurses until no buddy is free or the highest order is reached.
- The merged block is inserted into the appropriate free_list.

Correctness invariants:
- All free blocks at a given order have start addresses aligned to that order (start_address % block_size(order) == 0).
- Buddy computation is purely bitwise (XOR), which preserves correctness independent of earlier splits/merges.
- On deallocation, the allocator attempts to merge until maximal coalescing is achieved (subject to other allocations).

Design rationale:
- Separating Buddy allocator clarifies preconditions (power-of-two) and avoids complicating the general-purpose free-list code with order-based splitting logic.
- The XOR buddy computation is compact, efficient, and provably correct for buddy systems.

Complexity:
- Allocation worst-case requires finding a higher-order free block and performing O(log N) splits where N is total memory in units of minimal order.
- Deallocation may perform O(log N) merging steps.

## Cache Simulation Design

Scope:
- Multilevel cache: L1 and L2 levels are simulated. Each level is configurable in size, block size (cache line), and associativity.
- Replacement policy: FIFO per set (simple, deterministic).
- The simulator tracks hits, misses, hit ratios, and miss propagation between levels.

Data model:
- Cache address decomposition: for each level, an address is mapped to (tag, set_index, block_offset) according to the configured block size and number of sets (derived from cache size and associativity).
- Each cache set is represented as:
  - A small container of cache lines (associativity entries).
  - For replacement, a FIFO queue of indices or timestamps to determine the victim.
  - Each cache line stores: tag, valid bit (and optionally a 'last_loaded' counter for FIFO ordering).
- L1 and L2 are independent structures; L1 is checked first, then L2 on L1 miss.

Operation semantics:
- On a cache access (read or simulated access):
  1. Check L1:
     - If a line with matching tag and valid bit is present: record L1 hit.
     - Else: record L1 miss and check L2.
  2. Check L2:
     - If L2 hit: record L2 hit and perform the necessary data movement into L1 (bring the block into L1 according to FIFO replacement if L1 is full).
     - If L2 miss: record L2 miss and propagate the miss to "main memory" (conceptually). On miss, allocate the block into L2 (evict from L2 if necessary), and then into L1.
- Evictions:
  - FIFO within a set is used to select victims. On eviction, the evicted line is simply dropped (the simulator is metadata-only; write policy details are not modeled).
- Statistics:
  - Per-level: accesses, hits, misses, hit_ratio = hits / accesses.
  - Miss propagation: counts of accesses that miss L1 but hit L2, and those that miss both levels.

Design choices and rationale:
- FIFO replacement chosen for implementation simplicity and deterministic behavior in a teaching environment; while LRU is common in real caches, FIFO simplifies bookkeeping and keeps focus on multilevel behavior and miss propagation.
- Cache model is tag- and set-index-accurate; it supports configurable associativity to illustrate set conflicts.
- No attempt is made to simulate write policies or coherence; this keeps scope focused and avoids conflating allocator behavior with cache-write interactions.

Limitations (cache-specific):
- No differentiation between read/write semantics, no write-backs or dirty-bit handling.
- Replacement is FIFO, not LRU or adaptive policy.
- Timing/latency effects are not modeled beyond hit/miss counting.

## CLI / REPL Design

Purpose: Provide an interactive, scriptable interface for experiment-driven use and grading.

Command categories:
- Initialization
  - init <total_memory_bytes> [--mode allocator|buddy] [--buddy-size pow2_bytes]  
    Initializes the memory simulator. For the Buddy subsystem, `total_memory_bytes` must be a power of two and an explicit buddy-size/parameter can be provided.
- Allocator control
  - select allocator <first|best|worst>  
    Switch the general-purpose allocator at runtime; switching reorganizes the free-list representation but preserves current allocations as far as the design allows (note: selecting a different allocator does not retroactively change existing allocations' layouts).
  - select buddy  
    Switch into Buddy mode (valid only if preconditions met). Switching between general-purpose allocators and Buddy mode is allowed only when memory is in an initial or consistent state as enforced by the CLI (the implementation performs parameter checks).
- Allocation / Deallocation
  - malloc <size> -> returns block ID and start address on success
  - free <block_id> -> deallocates and reports success/failure
- Inspection and statistics
  - dump memory  (prints allocated blocks and free segments)
  - stats memory (used/free/utilization/external fragmentation)
  - dump cache
  - stats cache (L1/L2 hits/misses/hit ratios and miss propagation)
- Cache simulation
  - cache-configure <level> <size> <block_size> <associativity>
  - cache-access <address> (simulate an access and report hit/miss)
- Utility
  - help
  - quit / exit

Design and implementation notes:
- The CLI uses a simple parsing loop (REPL) with tokenization; commands are intentionally concise and deterministic for test scripting.
- Errors and invalid operations produce informative diagnostic messages to aid graders.
- The CLI enforces preconditions where necessary (e.g., buddy mode requires power-of-two total memory).

User model:
- Designed for an instructor or TA to run experiments interactively or by piping in scripts; commands output deterministic results based on the simulated metadata state.

## Build System and Portability

Build system:
- A Makefile builds the project targeting C++17. The Makefile sets standard compilation flags and produces a single executable suitable for classroom evaluation.

Portability considerations:
- Standard C++17 used; no platform-specific APIs are required.
- Code uses size_t and fixed-width integer types where appropriate to avoid implicit narrowing and to make address arithmetic explicit and portable.
- The build is tested on common Unix-like environments (Linux, WSL). The code avoids Windows-specific constructs; if porting to native Windows is desired, a POSIX shell or make-compatible environment is required.

Compiler recommendations:
- g++ or clang++ with -std=c++17. Recommended flags in the Makefile include -Wall -Wextra -Wpedantic -O2 for clean compilation and reasonable performance.

Testing:
- A set of unit and integration tests in tests/ exercise allocation patterns and cache scenarios. The test harness uses the REPL or direct function invocation depending on the test.

## Design Decisions and Trade-offs

1. Metadata-only memory model
   - Decision: simulate memory with metadata only.
   - Rationale: isolates allocator and fragmentation behavior from payload content. Reduces memory overhead and avoids unnecessary complexity.
   - Trade-off: cannot simulate per-byte data behavior, but statistics and fragmentation are accurately captured.

2. Separate Buddy subsystem
   - Decision: make Buddy allocator a separate module rather than an additional mode of the general allocator.
   - Rationale: Buddy system imposes strong invariants (power-of-two total size, order-aligned blocks) that are simpler to enforce in a dedicated implementation. This separation reduces cross-cutting complexity.
   - Trade-off: some duplication in bookkeeping structure but clearer semantics and easier testing.

3. Common Allocator interface for FF/BF/WF
   - Decision: runtime-switchable via polymorphism.
   - Rationale: facilitates direct comparison of strategies on the same workload without restarting the program (subject to state constraints).
   - Trade-off: retains a common set of capabilities only; some strategy-specific optimizations were deferred for clarity.

4. Choice of data structures
   - Decision: use lists and associative maps for free/allocated bookkeeping.
   - Rationale: lists make insertion/removal and address-ordered coalescing straightforward; maps provide fast lookup by block ID.
   - Trade-off: not optimized for extreme scale, but appropriate for assignment workloads and pedagogical clarity.

5. Cache FIFO replacement
   - Decision: implement FIFO replacement for simplicity.
   - Rationale: deterministic, easy to reason about for students; demonstrates set conflicts and associativity effects.
   - Trade-off: differs from common hardware LRU approximations, but the educational goal is preserved.

6. Single-threaded design
   - Decision: the simulator is single-threaded.
   - Rationale: concurrency introduces significant additional complexity (locks, atomicity, concurrent fragmentation effects) not required for the assignment.
   - Trade-off: this precludes evaluating allocator behavior under concurrent workloads.

## Limitations and Non-Goals

- Single-threaded only; no concurrency control or atomic operations on metadata.
- Metadata-only simulation: payload contents are not modeled; no byte-level writes or reads are performed.
- No memory protection, permissions, or address translation (virtual memory, paging, or TLBs are intentionally out of scope).
- Buddy allocator requires total memory to be a power of two; this is an enforced precondition.
- Cache simulation omits write-policy details (no write-back/write-through distinction), dirty bits, or coherence protocols.
- Timing and latency modeling is out of scope: the system reports counts and ratios, not time-based performance metrics.
- No simulation of allocator fragmentation due to allocator-internal metadata overheads—metadata has no in-band size overhead on simulated blocks.

## Possible Extensions (Optional)

These are presented as optional possibilities for future learning exercises only; they are not part of the current implementation.
- Add LRU or adaptive replacement policies to the cache simulator.
- Introduce configurable write policies and dirty-bit handling.
- Add concurrency and locking to explore allocator behavior under multi-threaded workloads.
- Provide a visualizer for memory layout and fragmentation over time.
- Support persistence of allocation traces for replay and offline analysis.

## Appendix: Implementation-level invariants and checks

- On every allocation and deallocation, the simulator recalculates and exposes statistics (used/free/utilization/external fragmentation) for consistency checks in tests.
- Allocation failures return explicit codes and do not change the state.
- For the Buddy subsystem, start addresses and block sizes are validated to ensure alignment invariant start_address % block_size == 0 before any merge operation.

## Conclusion

The Memory Management Simulator is organized to be pedagogical, deterministic, and inspectable. Design choices favor clarity and verifiability over micro-optimizations. The core subsystems (general-purpose allocators, Buddy allocator, and cache simulator) are decoupled to facilitate testing and comparison. The CLI enables both interactive experimentation and scripted evaluation by instructors and TAs.

For questions about specific implementation details or to review test cases and output traces, consult the repository's src/ and tests/ directories where the concrete classes and test harnesses are located.
