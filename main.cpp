#include <iostream>

#include "memory.h"

int main() {
    // Find the address of the System process in file "C:\Users\vanya\Documents\dump2.dmp"
    std::ptrdiff_t address = find_system_process_address("/Users/ivanlebedev/dev/Win11PhysMemoryParser/MEMORY.DMP");
    std::cout << "System _KPROCESS address: " << std::hex << address << '\n';
}
