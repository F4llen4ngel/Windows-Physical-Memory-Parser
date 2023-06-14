//
// Created by vanya on 6/10/2023.
//

#include  "memory.h"


bool validateKProcess(uint64_t kProcessAddress, std::ifstream &file) {

    _KPROCESS kProcess;
    readPhysicalMemory(kProcessAddress, &kProcess, sizeof(_KPROCESS), file);

    if (kProcess.DirectoryTableBase == 0x1ad000) {
        return true;
    }

    return false;
}

std::ptrdiff_t findSystemKProcessAddress(std::ifstream &file) {
    std::string data(std::istreambuf_iterator<char>(file), {});

    std::size_t position = data.find("System");
    while (position != std::string::npos) {
        auto kProcessOffset = position - 0x5a8; // 0x5a8 is the offset of ImageFileName within _EPROCESS structure

        if (validateKProcess(kProcessOffset, file)) {
            return static_cast<std::ptrdiff_t>(position - 0x5a8);
        }

        position = data.find("System", position + 1);
    }

    std::cerr << "'System' _EPROCESS not found\n";
    return -1;
}


bool readPhysicalMemory(uint64_t physicalAddress, void *buffer, size_t size, std::ifstream &file) {
    file.seekg(physicalAddress, std::ios::beg);

    if (file.fail()) {
        std::cerr << "Failed to seek to the physicalAddress position in the file\n";
        return false;
    }

    file.read(reinterpret_cast<char *>(buffer), size);

    if (file.fail()) {
        std::cerr << "Failed to read the physicalAddress from the file\n";
        return false;
    }

    return true;
}


uint64_t virtualToPhysicalAddress(uint64_t VirtualAddress, uint64_t DirectoryTableBase, std::ifstream &file) {
    VIRTUAL_ADDRESS virtAddr = {0};

    DIR_TABLE_BASE dirTableBase = {0};
    PML4E pml4e = {0};
    PDPTE pdpte = {0};
    PDPTE_LARGE pdpteLarge = {0};
    PDE pde = {0};
    PDE_LARGE pdeLarge = {0};
    PTE pte = {0};

    virtAddr.All = VirtualAddress;
    dirTableBase.All = DirectoryTableBase;

    if (!readPhysicalMemory(
            (dirTableBase.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.Pml4Index * 8),
            &pml4e,
            sizeof(PML4E),
            file)) {
        return 0;
    }

    if (pml4e.Bits.Present == 0) {
        std::cerr << "PML4E not present\n";
        return 0;
    }

    if (!readPhysicalMemory(
            (pml4e.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.PdptIndex * 8),
            &pdpte,
            sizeof(PDPTE),
            file)) {
        return 0;
    }

    if (pdpte.Bits.Present == 0) {
        std::cerr << "PDPTE not present\n";
        return 0;
    }

    if (IS_LARGE_PAGE(pdpte.All)) {
        pdpteLarge.All = pdpte.All;
        return (pdpteLarge.Bits.PhysicalAddress << PAGE_1GB_SHIFT) + PAGE_1GB_OFFSET(VirtualAddress);
    }

    if (!readPhysicalMemory(
            (pdpte.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.PdIndex * 8),
            &pde,
            sizeof(PDE),
            file)) {
        return 0;
    }

    if (pde.Bits.Present == 0) {
        std::cerr << "PDE not present\n";
        return 0;
    }

    if (IS_LARGE_PAGE(pde.All)) {
        pdeLarge.All = pde.All;

        return (pdeLarge.Bits.PhysicalAddress << PAGE_2MB_SHIFT) + PAGE_2MB_OFFSET(VirtualAddress);
    }

    if (!readPhysicalMemory(
            (pde.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + (virtAddr.Bits.PtIndex * 8),
            &pte,
            sizeof(PTE),
            file)) {
        return 0;
    }

    if (pte.Bits.Present == 0) {
        std::cerr << "PTE not present\n";
        return 0;
    }

    return (pte.Bits.PhysicalAddress << PAGE_4KB_SHIFT) + virtAddr.Bits.PageIndex;

}

uint64_t getNextProcessKProcess(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream &file) {
    uint64_t flinkVirtAddr;
    readPhysicalMemory(kProcessAddress + 0x448, &flinkVirtAddr, sizeof(uint64_t), file);
    uint64_t flinkPhysAddr = virtualToPhysicalAddress(flinkVirtAddr, DirectoryTableBase, file);

    uint64_t nextProcessKProcess = flinkPhysAddr - 0x448;

    return nextProcessKProcess;
}

uint64_t getPreviousProcessKProcess(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream &file) {
    uint64_t blinkVirtAddr;
    readPhysicalMemory(kProcessAddress + 0x448 + 0x8, &blinkVirtAddr, sizeof(uint64_t), file);
    uint64_t blinkPhysAddr = virtualToPhysicalAddress(blinkVirtAddr, DirectoryTableBase, file);

    uint64_t previousProcessKProcess = blinkPhysAddr - 0x448;

    return previousProcessKProcess;
}

std::string getProcessName(uint64_t kProcessAddress, std::ifstream &file) {
    char processName[15];
    readPhysicalMemory(kProcessAddress + 0x5a8, processName, 15, file);

    std::string processNameStr{processName};

    return processNameStr;
}

std::vector<Process> getProcessList(uint64_t systemKProcessAddress, uint64_t systemDirectoryTableBase, std::ifstream &file)
{
    std::vector<Process> processList;

    uint64_t curProcessKProcess = systemKProcessAddress;

    do {
        std::string curProcessName = getProcessName(curProcessKProcess, file);
        uint64_t curProcessDirectoryTableBase;
        readPhysicalMemory(curProcessKProcess + 0x28, &curProcessDirectoryTableBase, sizeof(uint64_t), file);

        Process curProcess{curProcessKProcess,
                           curProcessDirectoryTableBase,
                           curProcessName};
        processList.push_back(curProcess);

        curProcessKProcess = getNextProcessKProcess(curProcessKProcess, systemDirectoryTableBase, file);
    } while (curProcessKProcess != systemKProcessAddress);

    return processList;
}

void printNextProcessName(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream &file) {

    std::string curProcessName = getProcessName(kProcessAddress, file);
    uint64_t curProcessKProcess = kProcessAddress;

    do {
        std::cout << "Process: " << curProcessName << " _KPROCESS address: " << curProcessKProcess << '\n';
        uint64_t nextProcessKProcess = getNextProcessKProcess(curProcessKProcess, DirectoryTableBase, file);
        uint64_t nextProcessDirectoryTableBase;
        readPhysicalMemory(nextProcessKProcess + 0x28, &nextProcessDirectoryTableBase, sizeof(uint64_t), file);

        curProcessName = getProcessName(nextProcessKProcess, file);
        curProcessKProcess = nextProcessKProcess;
    } while (curProcessName != "System");

}

uint64_t getVadRootPhysicalAddress(uint64_t kProcessPhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    // read VadRoot virtual address from kProcess + 0x7d8;
    uint64_t vadRootVirtAddr;
    readPhysicalMemory(kProcessPhysAddr + 0x7d8, &vadRootVirtAddr, sizeof(uint64_t), file);
    uint64_t vadRootPhysAddr = virtualToPhysicalAddress(vadRootVirtAddr, DirectoryTableBase, file);

//    //
//    std::cout << "VadRoot virtual address: " << std::hex << vadRootVirtAddr << '\n';
//    std::cout << "VadRoot physical address: " << std::hex << vadRootPhysAddr << '\n';
//    //

    return vadRootPhysAddr;
}

uint64_t getLeftNodePhysicalAddress(uint64_t nodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t leftVirtAddr;
    readPhysicalMemory(nodePhysAddr, &leftVirtAddr, sizeof(uint64_t), file);

    if (leftVirtAddr == 0x0)
    {
        return 0;
    }

    uint64_t leftPhysAddr = virtualToPhysicalAddress(leftVirtAddr, DirectoryTableBase, file);

//    //
//    std::cout << "Left child virtual address: " << std::hex << leftVirtAddr << '\n';
//    std::cout << "Left child physical address: " << std::hex << leftPhysAddr << '\n';
//    //

    return leftPhysAddr;
}

uint64_t getRightNodePhysicalAddress(uint64_t nodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t rightVirtAddr;
    readPhysicalMemory(nodePhysAddr + 0x8, &rightVirtAddr, sizeof(uint64_t), file);

    if (rightVirtAddr == 0x0)
    {
        return 0;
    }

    uint64_t rightPhysAddr = virtualToPhysicalAddress(rightVirtAddr, DirectoryTableBase, file);

//    //
//    std::cout << "Right child virtual address: " << std::hex << rightVirtAddr << '\n';
//    std::cout << "Right child physical address: " << std::hex << rightPhysAddr << '\n';
//    //

    return rightPhysAddr;
}

uint64_t getParentNodePhysicalAddress(uint64_t nodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t parentValueVirtAddr;
    readPhysicalMemory(nodePhysAddr + 0x10, &parentValueVirtAddr, sizeof(uint64_t), file);
    parentValueVirtAddr = parentValueVirtAddr & (~0x7);
    uint64_t parentValuePhysAddr = virtualToPhysicalAddress(parentValueVirtAddr, DirectoryTableBase, file);

    return parentValuePhysAddr;
}

void printShit(uint64_t nodePhysicalAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t eProcessVirtualAddress;
    readPhysicalMemory(nodePhysicalAddress + 0x70, &eProcessVirtualAddress, sizeof(uint64_t), file);
    uint64_t eProcessPhysicalAddress = virtualToPhysicalAddress(eProcessVirtualAddress, DirectoryTableBase, file) - 1;
    uint64_t test_dtb;
    readPhysicalMemory(eProcessPhysicalAddress + 0x28, &test_dtb, sizeof(uint64_t), file);
    char test_name[15];
    readPhysicalMemory(eProcessPhysicalAddress + 0x5a8, &test_name, sizeof(test_name), file);

    std::cout << "VAD _KProcess: " << std::hex << eProcessPhysicalAddress << '\n';
    std::cout << "VAD dtb: " << std::hex << test_dtb << '\n';
    std::cout << "VAD name: " << test_name << '\n';

    ULONG StartingVpn, EndingVpn;
    UCHAR StartingVpnHigh, EndingVpnHigh;

    readPhysicalMemory(nodePhysicalAddress + 0x18, &StartingVpn, sizeof(ULONG), file);
    readPhysicalMemory(nodePhysicalAddress + 0x1c, &EndingVpn, sizeof(ULONG), file);
    readPhysicalMemory(nodePhysicalAddress + 0x20, &StartingVpnHigh, sizeof(UCHAR), file);
    readPhysicalMemory(nodePhysicalAddress + 0x21, &EndingVpnHigh, sizeof(UCHAR), file);

    uint64_t start = ((uint64_t)StartingVpn << 12) | ((uint64_t)StartingVpnHigh << 44);
    uint64_t end = (((uint64_t)EndingVpn + 1ll) << 12) | ((uint64_t)EndingVpnHigh << 44);

    std::cout << "VAD start-end: " << std::hex << start << " - " << std::hex << end << '\n';

    std::cout << "[====================================================]\n";
}

void travelVadTree(uint64_t nodePhysicalAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    //
    uint32_t nodeReferenceCount;
    readPhysicalMemory(nodePhysicalAddress + 0x24, &nodeReferenceCount, sizeof(uint32_t), file);
    printShit(nodePhysicalAddress, DirectoryTableBase, file);
    //

    uint64_t leftNode = getLeftNodePhysicalAddress(nodePhysicalAddress, DirectoryTableBase, file);
    uint64_t rightNode = getRightNodePhysicalAddress(nodePhysicalAddress, DirectoryTableBase, file);


    if (leftNode != 0x0)
    {
        travelVadTree(leftNode, DirectoryTableBase, file);
    }

    if (rightNode != 0x0)
    {
        travelVadTree(rightNode, DirectoryTableBase, file);
    }
}

void testShit(uint64_t kProcessPhysicalAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    kProcessPhysicalAddress = getNextProcessKProcess(kProcessPhysicalAddress, DirectoryTableBase, file);
    readPhysicalMemory(kProcessPhysicalAddress + 0x28, &DirectoryTableBase, sizeof(uint64_t), file);

    uint64_t vadCount;
    readPhysicalMemory(kProcessPhysicalAddress + 0x7e8, &vadCount, sizeof(uint64_t), file);

    std::cout << "VadCount from _EPROCESS: " << vadCount << '\n';

    uint64_t vadRoot = getVadRootPhysicalAddress(kProcessPhysicalAddress, DirectoryTableBase, file);
    std::cout << "VadRoot physical address: " << std::hex << vadRoot << '\n';

    uint64_t leftNode = getLeftNodePhysicalAddress(vadRoot, DirectoryTableBase, file);
    uint64_t rightNode = getRightNodePhysicalAddress(vadRoot, DirectoryTableBase, file);
    std::cout << "VadRoot left child physical address: " << std::hex << leftNode << '\n';
    std::cout << "VadRoot right child physical address: " << std::hex << rightNode << '\n';

    uint64_t testNodeLeft = getParentNodePhysicalAddress(leftNode, DirectoryTableBase, file);
    uint64_t testNodeRight = getParentNodePhysicalAddress(rightNode, DirectoryTableBase, file);
    std::cout << "Left node's parent: " << std::hex << testNodeLeft << '\n';
    std::cout << "Right node's parent: " << std::hex << testNodeRight << '\n';

    // test _MMVAD shit
    uint32_t rootReferenceCount, leftReferenceCount, rightReferenceCount;
    readPhysicalMemory(vadRoot + 0x24, &rootReferenceCount, sizeof(uint32_t), file);
    readPhysicalMemory(leftNode + 0x24, &leftReferenceCount, sizeof(uint32_t), file);
    readPhysicalMemory(rightNode + 0x24, &rightReferenceCount, sizeof(uint32_t), file);
    std::cout << "VadRoot reference count: " << rootReferenceCount << '\n';
    std::cout << "Left node reference count: " << leftReferenceCount << '\n';
    std::cout << "Right node referenct count: " << rightReferenceCount << '\n';

    std::cout << "----------------------------------------\n";
    travelVadTree(vadRoot, DirectoryTableBase, file);

}