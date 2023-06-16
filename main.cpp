#include <iostream>

#include "memory.h"

int main() {
    // Find the address of the System process in file "C:\Users\vanya\Documents\dump2.dmp"
    auto filename = "/Users/ivanlebedev/dev/Win11PhysMemoryParser/2.raw";
    // read file ifstream
    std::ifstream file(filename, std::ios::binary);
    std::ptrdiff_t address = findSystemKProcessAddress(file);
    std::cout << "System _KPROCESS address: " << std::hex << address << '\n';
    // read file ifstream
    uint64_t dtb;
    readPhysicalMemory(address + 0x28, &dtb, sizeof(uint64_t), file);
    std::cout << "System DirectoryTableBase: " << std::hex << dtb << '\n';
    std::vector<Process> processes = getProcessList(address, dtb, file);
    std::cout << "------------------------------------------\n";
    for (auto &process : processes) {
        std::cout << process.ProcessName <<  " " << process.KProcessAddress << " " << process.DirectoryTableBase << '\n';
    }
    std::cout << "------------------------------------------\n";

    for (auto &process : processes) {
        if (process.ProcessName == "dudecheck.exe") {
            std::cout << "Found dudecheck.exe\n";
            std::cout << "KProcessAddress: " << std::hex << process.KProcessAddress << '\n';
            std::cout << "DirectoryTableBase: " << std::hex << process.DirectoryTableBase << '\n';
            std::cout << "------------------------------------------\n";
            address = process.KProcessAddress;
            dtb = process.DirectoryTableBase;
            break;
        }
    }
    if (dtb == _CR3) {
        std::cout << "dude not found\n";
        return 0;
    }
    std::vector dudeNodes = readProcessVadTree(address, dtb, file);

    for (auto &node : dudeNodes) {
        std::cout << "VadNode: " << std::hex << node.startAddress << " - " << node.endAddress << '\n';
    }
}
