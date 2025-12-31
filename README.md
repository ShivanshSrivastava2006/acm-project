Memory Management Simulator
Overview

This project is a C++-based Memory Management Simulator that models how an operating system manages physical memory. It simulates dynamic memory allocation and deallocation, supports multiple allocation strategies, and reports fragmentation and utilization metrics through an interactive command-line interface.

The simulator is a user-space model, not an OS kernel. Its goal is to accurately represent OS-level memory management behavior using well-defined data structures and algorithms.

Features

Simulation of contiguous physical memory

Dynamic memory allocation and deallocation

Allocation strategies:

First Fit

Best Fit

Worst Fit

Block splitting and coalescing to reduce fragmentation

Interactive CLI (REPL-based)

Memory layout visualization

Memory utilization and fragmentation statistics

Memory Model

Physical memory is simulated as a contiguous, byte-addressable region

Memory is represented using metadata blocks rather than actual data storage

Each block stores:

Start address

Size

Allocation status (free/used)

Allocation ID

Adjacent free blocks are merged automatically during deallocation

Allocation Strategies

First Fit: Selects the first free block large enough for the request

Best Fit: Selects the smallest free block that satisfies the request

Worst Fit: Selects the largest available free block

Allocation policies are separated from memory management logic using an allocator abstraction, enabling clean extensibility.

Statistics

The simulator computes and reports:

Total memory

Used memory

Free memory

Memory utilization (%)

External fragmentation (%)

External fragmentation is calculated as:

1 − (largest_free_block / total_free_memory)

Command-Line Interface

Supported commands:

init memory <size>
set allocator <first_fit | best_fit | worst_fit>
malloc <size>
free <block_id>
dump
stats
exit


Example session:

> init memory 1024
> set allocator first_fit
> malloc 100
> malloc 200
> dump
> stats

Project Structure
src/
├── core/           # Memory model and block management
├── allocator/      # Allocation strategies
├── cli/            # Interactive REPL
├── main.cpp
include/
tests/
docs/

Build Instructions

Compile using g++:

g++ -std=c++17 src/main.cpp src/cli/repl.cpp src/core/memory.cpp \
src/allocator/first_fit.cpp src/allocator/best_fit.cpp src/allocator/worst_fit.cpp \
-o memsim


Run:

./memsim

Limitations

Does not simulate actual data storage

Virtual memory, paging, and cache simulation are not implemented

Designed for educational simulation, not production use

Purpose

This project aims to provide hands-on understanding of:

OS memory allocation strategies

Fragmentation behavior

Systems-level design and abstraction

✅ Status

Core functionality complete and fully tested.
