//
// Created by vanya on 6/10/2023.
//

#include  "memory.h"


bool validateKProcess(uint64_t kProcessAddress, std::ifstream& file)
{
    file.seekg(kProcessAddress, std::ios::beg);

    if (file.fail())
    {
        std::cerr << "Failed to seek to the _KPROCESS position in the file\n";
        return false;
    }

    _KPROCESS kProcess;
    readPhysicalMemory(kProcessAddress, &kProcess, sizeof(_KPROCESS), file);

    if (kProcess.DirectoryTableBase == 0x1ad000)
    {
        return true;
    }

    return false;
}

std::ptrdiff_t findSystemKProcessAddress(std::ifstream& file)
{
    std::string data(std::istreambuf_iterator<char>(file), {});

    std::size_t position = data.find("System");
    while (position != std::string::npos)
    {
        auto kProcessOffset = position - 0x5a8; // 0x5a8 is the offset of ImageFileName within _EPROCESS structure

        if (validateKProcess(kProcessOffset, file))
        {
            return static_cast<std::ptrdiff_t>(position - 0x5a8);
        }

        position = data.find("System", position + 1);
    }

    std::cerr << "'System' _EPROCESS not found\n";
    return -1;
}


bool readPhysicalMemory(uint64_t physicalAddress, void* buffer, size_t size, std::ifstream& file)
{
    file.seekg(physicalAddress, std::ios::beg);

    if (file.fail())
    {
        std::cerr << "Failed to seek to the physicalAddress position in the file\n";
        return false;
    }

    file.read(reinterpret_cast<char*>(buffer), size);

    if (file.fail())
    {
        std::cerr << "Failed to read the physicalAddress from the file\n";
        return false;
    }

    return true;
}


uint64_t virtualToPhysicalAddress(uint64_t VirtualAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    VIRTUAL_ADDRESS virtAddr     = { 0 };

    DIR_TABLE_BASE  dirTableBase = { 0 };
    PML4E           pml4e        = { 0 };
    PDPTE           pdpte        = { 0 };
    PDPTE_LARGE     pdpteLarge   = { 0 };
    PDE             pde          = { 0 };
    PDE_LARGE       pdeLarge     = { 0 };
    PTE             pte          = { 0 };

    virtAddr.All = VirtualAddress;
    dirTableBase.All = DirectoryTableBase;

    if (!readPhysicalMemory(
            (dirTableBase.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.Pml4Index * 8),
            &pml4e,
            sizeof(PML4E),
            file))
    {
        return 0;
    }

    std::cout << "PML4E: " << std::hex << pml4e.All << '\n'; // TODO: remove this line

    if (pml4e.Bits.Present == 0)
    {
        std::cerr << "PML4E not present\n";
        return 0;
    }

    if (!readPhysicalMemory(
            (pml4e.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.PdptIndex * 8),
            &pdpte,
            sizeof(PDPTE),
            file))
    {
        return 0;
    }

    std::cout << "PDPTE: " << std::hex << pdpte.All << '\n'; // TODO: remove this line

    if ( pdpte.Bits.Present == 0 )
    {
        std::cerr << "PDPTE not present\n";
        return 0;
    }

    if (IS_LARGE_PAGE(pdpte.All))
    {
        pdpteLarge.All = pdpte.All;
        return (pdpteLarge.Bits.PhysicalAddress << PAGE_1GB_SHIFT) + PAGE_1GB_OFFSET(VirtualAddress);
    }

    if (!readPhysicalMemory(
            (pdpte.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.PdIndex * 8),
            &pde,
            sizeof(PDE),
            file))
    {
        return 0;
    }

    std::cout << "PDE: " << std::hex << pde.All << '\n'; // TODO: remove this line

    if (pde.Bits.Present == 0)
    {
        std::cerr << "PDE not present\n";
        return 0;
    }

    if (IS_LARGE_PAGE(pde.All))
    {
        pdeLarge.All = pde.All;

        return (pdeLarge.Bits.PhysicalAddress << PAGE_2MB_SHIFT) + PAGE_2MB_OFFSET( VirtualAddress );
    }

    if (!readPhysicalMemory(
            (pde.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.PtIndex * 8),
            &pte,
            sizeof(PTE),
            file))
    {
        return 0;
    }

    std::cout << "PTE: " << std::hex << pte.All << '\n'; // TODO: remove this line

    if ( pte.Bits.Present == 0 )
    {
        std::cerr << "PTE not present\n";
        return 0;
    }

    return (pte.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + virtAddr.Bits.PageIndex;

}


void printNextProcessName(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    // read flinkVirtAddr = ActiveProcessLinks.Flink (KProcessAddress + 0x448)
    uint64_t flinkVirtAddr;
    readPhysicalMemory(kProcessAddress + 0x448, &flinkVirtAddr, sizeof(uint64_t), file);
    std::cout << "[DEBUG] flinkVirtAddr: " << std::hex << flinkVirtAddr << '\n'; // TODO: remove this line
    // read flinkPhysAddr = virtualToPhysicalAddress(flinkVirtAddr, DirectoryTableBase)
    uint64_t flinkPhysAddr = virtualToPhysicalAddress(flinkVirtAddr, DirectoryTableBase, file);
    std::cout << "[DEBUG] flinkPhysAddr: " << std::hex << flinkPhysAddr << '\n'; // TODO: remove this line
    char* nextProcessName = new char[15];
    readPhysicalMemory(flinkPhysAddr - 0x448 + 0x5a8, nextProcessName, 15, file);
    std::cout << "[DEBUG] nextProcessName: " << nextProcessName << '\n'; // TODO: remove this line

}
