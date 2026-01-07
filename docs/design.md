# Memory Management Simulator — Design Document

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
      - BuddyAllocator (se

