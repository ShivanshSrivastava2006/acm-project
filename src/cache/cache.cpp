#include "cache.h"
#include <iostream>

Cache::Cache(size_t csize, size_t bsize, int assoc)
    : cache_size(csize),
      block_size(bsize),
      associativity(assoc),
      hits(0),
      misses(0)
{
    num_sets = cache_size / (block_size * associativity);
    sets.reserve(num_sets);
    for (int i = 0; i < num_sets; i++)
        sets.emplace_back(associativity);
}

bool Cache::access(size_t address) {
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    CacheSet& set = sets[set_index];

    // check hit
    for (auto& line : set.lines) {
        if (line.valid && line.tag == tag) {
            hits++;
            return true;
        }
    }

    // miss → FIFO replace
    misses++;

    CacheLine& victim = set.lines[set.fifo_ptr];
    victim.valid = true;
    victim.tag = tag;

    set.fifo_ptr = (set.fifo_ptr + 1) % associativity;

    return false;
}

void Cache::reset_stats() {
    hits = misses = 0;
}

size_t Cache::get_hits() const { return hits; }
size_t Cache::get_misses() const { return misses; }

double Cache::hit_ratio() const {
    size_t total = hits + misses;
    return total == 0 ? 0.0 : (double)hits / total;
}

void Cache::dump(const std::string& name) const {
    std::cout << name << " Cache Stats:\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Misses: " << misses << "\n";
    std::cout << "Hit ratio: " << hit_ratio() * 100 << "%\n";
}
