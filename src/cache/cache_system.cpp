#include "cache_system.h"
#include <iostream>

CacheSystem::CacheSystem(size_t l1_size, size_t l1_block, int l1_assoc,
                         size_t l2_size, size_t l2_block, int l2_assoc)
    : l1(l1_size, l1_block, l1_assoc),
      l2(l2_size, l2_block, l2_assoc),
      memory_accesses(0) {}

void CacheSystem::access(size_t address) {
    if (l1.access(address))
        return;

    if (l2.access(address))
        return;

    // miss in both → memory access
    memory_accesses++;
}

void CacheSystem::dump_stats() const {
    l1.dump("L1");
    l2.dump("L2");
    std::cout << "Memory accesses: " << memory_accesses << "\n";
}

void CacheSystem::reset() {
    l1.reset_stats();
    l2.reset_stats();
    memory_accesses = 0;
}
