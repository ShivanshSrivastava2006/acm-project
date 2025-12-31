#include "core/memory.h"
#include <iostream>

int main() {
    Memory mem;
    mem.init(1024);

    int a = mem.allocate(100);
    int b = mem.allocate(200);

    mem.dump();

    std::cout << "Freeing block " << a << "\n";
    mem.deallocate(a);

    mem.dump();
}
