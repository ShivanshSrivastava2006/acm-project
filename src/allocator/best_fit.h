#ifndef BEST_FIT_H
#define BEST_FIT_H

#include "allocator.h"

class BestFitAllocator : public Allocator {
public:
    std::list<Block>::iterator
    select_block(std::list<Block>& blocks, size_t size) override;
};

#endif
