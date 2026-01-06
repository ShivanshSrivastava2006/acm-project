#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstddef>
#include <string>
#include "cache_set.h"

class Cache {
private:
    size_t cache_size;
    size_t block_size;
    int associativity;
    int num_sets;

    std::vector<CacheSet> sets;

    // stats
    size_t hits;
    size_t misses;

public:
    Cache(size_t cache_size,
          size_t block_size,
          int associativity);

    bool access(size_t address);   // true = hit, false = miss
    void reset_stats();

    size_t get_hits() const;
    size_t get_misses() const;
    double hit_ratio() const;

    void dump(const std::string& name) const;
};

#endif
