# Memory Management Simulator

A command-line memory management simulator that models a contiguous physical memory region and multiple allocation strategies. The simulator focuses on metadata-only simulation, allocation policies, fragmentation analysis, and interactive control to help demonstrate operating system memory management concepts and compare allocator behavior.

## Table of contents

- Overview
- Key features
- CLI commands and usage
- Allocation strategies
- Memory visualization and statistics
- Buddy allocator (planned)
- Architecture and design goals
- Typical use cases
- Contributing
- License
- One-line summary

## Overview

This project simulates physical memory and multiple allocation policies. It does not store application data inside simulated memory. Instead, it maintains metadata that represents blocks, addresses, allocation IDs, and free lists. The simulator is focused on teaching and demonstrating allocation mechanisms, fragmentation, and allocator performance.

## Key features

Core features that are already implemented

1. Physical memory simulation
   - Simulates a contiguous block of physical memory.
   - Memory is byte-addressable.
   - No real payload is stored. The system manages simulation metadata only.

2. Dynamic memory allocation
   - Supports runtime allocation and deallocation of variable-sized blocks.
   - Allocation requests are validated and handled via metadata-only operations.
   - Memory is divided into variable-sized blocks tracked by metadata.

3. Allocation strategies (contiguous allocation)
   - Clean allocator abstraction supports multiple strategies:
     - First fit
     - Best fit
     - Worst fit
   - Each strategy searches free blocks according to its policy.
   - Blocks can be split on allocation to satisfy requests.
   - Allocation strategies plug into the core memory model without modifying it.

4. Deallocation and coalescing
   - Frees memory by block ID assigned at allocation time.
   - Automatically merges adjacent free blocks to reduce fragmentation.
   - Prevents uncontrolled external fragmentation by coalescing.

5. Interactive command-line interface (CLI / REPL)
   - Commands are parsed robustly and invalid input does not crash the program.
   - The allocator policy can be switched at runtime.
   - Supported commands:
     - `init memory <size>`
     - `set allocator <first_fit | best_fit | worst_fit>`
     - `malloc <size>`
     - `free <block_id>`
     - `dump`
     - `stats`
     - `exit`

6. Memory visualization
   - Displays memory layout with address ranges, block status (FREE or USED), and allocation IDs.
   - Visualization helps to make fragmentation and block placement visible.

7. Memory statistics and metrics
   - `stats` command computes and displays:
     - Total memory
     - Used memory
     - Free memory
     - Memory utilization (percentage)
     - External fragmentation (percentage)
   - External fragmentation formula used:
     - 1 - (largest_free_block / total_free_memory)

8. Clean modular architecture
   - Clear separation of concerns:
     - Core memory model
     - Allocation policies
     - CLI logic
     - Statistics and metrics
   - Designed for easy extension without breaking existing code.
   - No circular dependencies across modules.

Optional / advanced feature in progress

9. Buddy allocation system (planned)
   - A separate memory management subsystem that does not modify the existing contiguous allocators.
   - Characteristics and requirements:
     - Total memory size must be a power of two.
     - Allocation sizes are rounded up to the nearest power of two.
     - Maintains free lists per block order.
     - Allocations use recursive splitting of blocks.
     - Deallocation performs buddy merging using XOR logic.
     - Provides very low external fragmentation and demonstrates a different performance and fragmentation profile.
   - Buddy allocator will be pluggable from the CLI to allow comparisons between internal and external fragmentation behavior.

## CLI commands and usage

Start the simulator to enter an interactive REPL. Once in the REPL, use the following commands:

- Initialize memory
  ```
  init memory <size>
  ```
  Example:
  ```
  init memory 65536
  ```
  Initializes the simulated physical memory to 65,536 bytes.

- Set allocator policy
  ```
  set allocator <first_fit | best_fit | worst_fit>
  ```
  Example:
  ```
  set allocator first_fit
  ```

- Allocate memory
  ```
  malloc <size>
  ```
  Example:
  ```
  malloc 1024
  ```
  Returns an allocation ID that can be used for freeing.

- Free memory
  ```
  free <block_id>
  ```
  Example:
  ```
  free 3
  ```

- Inspect memory layout
  ```
  dump
  ```
  Shows address ranges, block size, status, and allocation IDs.

- Show statistics
  ```
  stats
  ```
  Shows totals, utilization percent, and external fragmentation percent.

- Exit REPL
  ```
  exit
  ```

The CLI parser is defensive. Invalid commands or parameters produce helpful error messages and keep the REPL running.

## Allocation strategies - behavior details

- First fit
  - Scans free blocks from the beginning and selects the first block large enough to satisfy the request.
  - Splits a larger block into an allocated block and a smaller free block when needed.

- Best fit
  - Searches all free blocks and selects the smallest free block that is large enough for the request.
  - May reduce wasted space in the chosen block, but can produce smaller leftover fragments.

- Worst fit
  - Chooses the largest available free block for the allocation.
  - Intends to leave reasonably sized free blocks behind, but behavior depends on workload.

All strategies operate via the same allocator interface so they are interchangeable at runtime and do not modify the core memory model.

## Memory visualization and statistics

The `dump` command prints a succinct representation for each block such as:

- Address range (start - end)
- Block size in bytes
- Status: FREE or USED
- Allocation ID for used blocks

The `stats` command prints:
- Total memory (bytes)
- Used memory (bytes)
- Free memory (bytes)
- Memory utilization: used / total expressed as a percentage
- External fragmentation: computed as 1 - (largest_free_block / total_free_memory)

These outputs are intended to help compare allocator behavior and observe fragmentation dynamics as allocations and frees proceed.

## Buddy allocator (planned)

The Buddy allocator will be implemented as a separate allocator module with the following properties:
- Requires that the initial memory size is a power of two.
- Rounds allocation requests up to the nearest power of two.
- Maintains a set of free lists keyed by block order.
- Allocates by recursively splitting larger blocks when needed.
- Deallocates by merging free buddies using XOR logic.
- Will be selectable from the CLI like other allocators.
- Will provide contrast between internal fragmentation and external fragmentation compared to the existing allocators.

## Architecture and design goals

- Policy versus mechanism separation
  - The project separates the memory mechanism from allocator policies. Allocators implement a defined interface and can be swapped without touching core memory logic.

- Modularity
  - Core modules include:
    - Memory model and block metadata
    - Allocator interface and implementations
    - CLI and command parsing
    - Statistics and reporting
  - Modules should be easy to extend or replace.

- Educational clarity
  - Code and output emphasize clarity, observability, and repeatability for use in coursework and demonstrations.

## Typical use cases

- Operating systems coursework and labs
- Systems programming portfolio and demos
- Algorithmic studies and comparison of allocation strategies
- Demonstrations of fragmentation and allocator tradeoffs

## Contributing

Contributions are welcome. Typical contributions include:
- Bug reports and test cases
- Improvements to the CLI and visualization
- New allocator implementations such as the Buddy allocator or slab allocator
- Documentation and examples

Please open issues and pull requests in the repository. Provide reproducible steps for bugs and a description of proposed changes for PRs.

## License

Add your preferred license information here or include a LICENSE file in the repository.

## One-line summary

A memory management simulator with multiple allocation strategies, fragmentation analysis, interactive control, and a pluggable Buddy allocator extension, built with clean systems-level abstractions.
