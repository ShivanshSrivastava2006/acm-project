#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <vector>
#include <list>
#include <cstddef>
#include "buddy_block.h"

class BuddyAllocator {
private:
    size_t total_size;
    int max_order;

    // free_lists[k] holds free blocks of size 2^k
    std::vector<std::list<BuddyBlock>> free_lists;

public:
    BuddyAllocator();

    // initialize memory (size must be power of two)
    bool init(size_t size);

    // allocate memory, returns starting address or -1 on failure
    long long allocate(size_t size);

    // free memory given address and size
    bool deallocate(size_t addr, size_t size);

    // debugging / visualization
    void dump() const;
};

#endif
