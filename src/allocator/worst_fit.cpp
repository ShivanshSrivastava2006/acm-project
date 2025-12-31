#include "worst_fit.h"

std::list<Block>::iterator
WorstFitAllocator::select_block(std::list<Block>& blocks, size_t size) {
    auto worst = blocks.end();

    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->free && it->size >= size) {
            if (worst == blocks.end() || it->size > worst->size)
                worst = it;
        }
    }
    return worst;
}
