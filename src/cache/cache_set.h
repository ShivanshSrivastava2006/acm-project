#ifndef CACHE_SET_H
#define CACHE_SET_H

#include <vector>
#include "cache_line.h"

struct CacheSet {
    std::vector<CacheLine> lines;
    int fifo_ptr;

    CacheSet(int associativity)
        : lines(associativity), fifo_ptr(0) {}
};

#endif
