#include "first_fit.h"

std::list<Block>::iterator
FirstFitAllocator::select_block(std::list<Block>& blocks, size_t size) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->free && it->size >= size)
            return it;
    }
    return blocks.end();
}
