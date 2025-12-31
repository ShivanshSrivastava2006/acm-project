#ifndef WORST_FIT_H
#define WORST_FIT_H

#include "allocator.h"

class WorstFitAllocator : public Allocator {
public:
    std::list<Block>::iterator
    select_block(std::list<Block>& blocks, size_t size) override;
};

#endif
