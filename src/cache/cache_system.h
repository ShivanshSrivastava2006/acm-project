#ifndef CACHE_SYSTEM_H
#define CACHE_SYSTEM_H

#include "cache.h"

class CacheSystem {
private:
    Cache l1;
    Cache l2;

    size_t memory_accesses;

public:
    CacheSystem(size_t l1_size, size_t l1_block, int l1_assoc,
                size_t l2_size, size_t l2_block, int l2_assoc);

    void access(size_t address);

    void dump_stats() const;
    void reset();
};

#endif
