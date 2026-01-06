#ifndef BUDDY_BLOCK_H
#define BUDDY_BLOCK_H

#include <cstddef>

struct BuddyBlock {
    size_t start;   // starting address
    int order;      // block size = 2^order
};

#endif
