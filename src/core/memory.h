#ifndef MEMORY_H
#define MEMORY_H

#include <list>
#include <cstddef>

struct Block {
    size_t start;   
    size_t size;    
    bool free;      
    int id;         
};

class Memory {
private:
    size_t total_size;
    std::list<Block> blocks;
    int next_id;

public:
    Memory();

    
    void init(size_t size);

    
    int allocate(size_t size);

    
    bool deallocate(int id);

    
    void dump() const;
};

#endif
