#include "best_fit.h"

std::list<Block>::iterator
BestFitAllocator::select_block(std::list<Block>& blocks, size_t size) {
    auto best = blocks.end();

    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->free && it->size >= size) {
            if (best == blocks.end() || it->size < best->size)
                best = it;
        }
    }
    return best;
}
