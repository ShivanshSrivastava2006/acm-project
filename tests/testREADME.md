# Test Workloads

These workloads are designed to validate allocator correctness,
fragmentation behavior, and buddy allocator merging logic.

Exact numeric outputs may differ depending on allocator state,
but the following properties must hold:

- No overlapping blocks
- All free blocks are coalesced when possible
- Fragmentation percentage increases when non-contiguous free blocks exist
- Buddy allocator merges recursively when buddies are free
- Cache statistics reflect correct hit/miss propagation
