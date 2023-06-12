#include <iostream>

#include "memory.h"

int main() {
    // Find the address of the System process in file "C:\Users\vanya\Documents\dump2.dmp"
    auto filename = "Z:\\dev\\Win11PhysMemoryParser\\physmem3.raw";
    // read file ifstream
    std::ifstream file(filename, std::ios::binary);
    std::ptrdiff_t address = findSystemKProcessAddress(file);
    std::cout << "System _KPROCESS address: " << std::hex << address << '\n';
    // read file ifstream
    uint64_t dtb;
    readPhysicalMemory(address + 0x28, &dtb, sizeof(uint64_t), file);
    std::cout << "System DirectoryTableBase: " << std::hex << dtb << '\n';
    printNextProcessName(address, dtb, file);
}
