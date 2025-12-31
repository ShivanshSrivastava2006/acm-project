#include "core/memory.h"
#include "allocator/worst_fit.h"

int main() {
    Memory mem;
    WorstFitAllocator alloc;

    mem.init(1024);
    mem.set_allocator(&alloc);

    int a = mem.allocate(100);
    int b = mem.allocate(200);

    mem.dump();
    mem.deallocate(a);
    mem.dump();
}
