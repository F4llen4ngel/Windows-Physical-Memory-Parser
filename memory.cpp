//
// Created by vanya on 6/10/2023.
//

#include  "memory.h"


bool validateKProcess(uint64_t kProcessAddress, std::ifstream &file) {
    file.seekg(kProcessAddress, std::ios::beg);

    if (file.fail()) {
        std::cerr << "Failed to seek to the _KPROCESS position in the file\n";
        return false;
    }

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
    uint64_t nextProcessDirectoryTableBase;
    readPhysicalMemory(nextProcessKProcess + 0x28, &nextProcessDirectoryTableBase, sizeof(uint64_t), file);

    // shit check
    std::cout << "System(?) KProcess address: " << getPreviousProcessKProcess(nextProcessKProcess, nextProcessDirectoryTableBase, file) << '\n';

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

uint64_t getLeftNodePhysicalAddress(_RTL_BALANCED_NODE node, uint64_t DirectoryTableBase, std::ifstream &file) {
    auto leftNodeVirtAddr = reinterpret_cast<uint64_t>(node.Left);
    uint64_t leftNodePhysicalAddr = virtualToPhysicalAddress(leftNodeVirtAddr, DirectoryTableBase, file);
    return leftNodePhysicalAddr;
}

uint64_t getRightNodePhysicalAddress(_RTL_BALANCED_NODE node, uint64_t DirectoryTableBase, std::ifstream &file) {
    auto rightNodeVirtAddr = reinterpret_cast<uint64_t>(node.Right);
    uint64_t rightNodePhysicalAddr = virtualToPhysicalAddress(rightNodeVirtAddr, DirectoryTableBase, file);
    return rightNodePhysicalAddr;
}

uint64_t getParentNodePhysicalAddress(_RTL_BALANCED_NODE node, uint64_t DirectoryTableBase, std::ifstream &file) {
    auto parentNodeVirtAddr = reinterpret_cast<uint64_t>(node.ParentValue) & (~(0x3));
    uint64_t parentNodePhysicalAddr = virtualToPhysicalAddress(parentNodeVirtAddr, DirectoryTableBase, file);
    return parentNodePhysicalAddr;
}

uint64_t getVadRootPhysAddress(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream &file) {
    uint64_t vadRootVirtAddr;
    readPhysicalMemory(kProcessAddress + 0x7d8, &vadRootVirtAddr, sizeof(uint64_t), file);

    uint64_t vadRootPhysAddr = virtualToPhysicalAddress(vadRootVirtAddr, DirectoryTableBase, file);

    return vadRootPhysAddr;
}

void readNodeMemoryPages(uint64_t curNodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    ULONG StartingVpn, EndingVpn;
    UCHAR StartingVpnHigh, EndingVpnHigh;

    readPhysicalMemory(curNodePhysAddr + 0x18, &StartingVpn, sizeof(ULONG), file);
    readPhysicalMemory(curNodePhysAddr + 0x1c, &EndingVpn, sizeof(ULONG), file);

    readPhysicalMemory(curNodePhysAddr + 0x20, &StartingVpnHigh, sizeof(UCHAR), file);
    readPhysicalMemory(curNodePhysAddr + 0x21, &EndingVpnHigh, sizeof(UCHAR), file);

    // https://github.com/volatilityfoundation/volatility/blob/master/volatility/plugins/overlays/windows/vad_vtypes.py#L476
    uint64_t startingVpnVirtualAddress = (((uint64_t)StartingVpn) << 12) | (((uint64_t)StartingVpnHigh) << 44);
    uint64_t endingVpnVirtualAddress = (((uint64_t)EndingVpn) << 12) | (((uint64_t)EndingVpnHigh) << 44);

    std::cout << "Starting VPN: " << std::hex << startingVpnVirtualAddress << " Ending VPN: " << endingVpnVirtualAddress << '\n';

    uint64_t startingVpnPhysicalAddress = virtualToPhysicalAddress(startingVpnVirtualAddress, DirectoryTableBase, file);
    uint64_t endingVpnPhysicalAddress = virtualToPhysicalAddress(endingVpnVirtualAddress, DirectoryTableBase, file);

    std::cout << "Starting VPN physical address: " << std::hex << startingVpnPhysicalAddress << " Ending VPN physical address: " << endingVpnPhysicalAddress << '\n';

    uint64_t numPages = (endingVpnVirtualAddress - startingVpnVirtualAddress) / 4096;

    std::vector<Page> pages;

    for (int64_t i = 0; i < numPages; ++i) {
        uint64_t pageVirtualAddress = startingVpnVirtualAddress + i * 4096;
        uint64_t pagePhysicalAddress = virtualToPhysicalAddress(pageVirtualAddress, DirectoryTableBase, file);
        std::cout << "Page virtual address: " << std::hex << pageVirtualAddress << " Page physical address: " << pagePhysicalAddress << '\n';
        Page page{};
        readPhysicalMemory(pagePhysicalAddress, &page, sizeof(Page), file);
        pages.push_back(page);
        std::cout << "Page buffer: " << std::hex << page.buffer << '\n';
    }

}

void traverseVadTree(uint64_t curNodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    _RTL_BALANCED_NODE curNode{};
    readPhysicalMemory(curNodePhysAddr, &curNode, sizeof(_RTL_BALANCED_NODE), file);

}

void getProcessMemorySpace(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t vadRootPhysAddr = getVadRootPhysAddress(kProcessAddress, DirectoryTableBase, file);

    std::cout << "Vad root physical address: " << std::hex << vadRootPhysAddr << '\n';

    _RTL_BALANCED_NODE vadRoot{};
    readPhysicalMemory(vadRootPhysAddr, &vadRoot, sizeof(_RTL_BALANCED_NODE), file);


    std::cout << "Left node: " << getLeftNodePhysicalAddress(vadRoot, DirectoryTableBase, file) << '\n';
    std::cout << "Right node: " << getRightNodePhysicalAddress(vadRoot, DirectoryTableBase, file) << '\n';

    ULONG StartingVpn, EndingVpn;
    UCHAR StartingVpnHigh, EndingVpnHigh;

    readPhysicalMemory(vadRootPhysAddr + 0x18, &StartingVpn, sizeof(ULONG), file);
    readPhysicalMemory(vadRootPhysAddr + 0x1c, &EndingVpn, sizeof(ULONG), file);
    readPhysicalMemory(vadRootPhysAddr + 0x20, &StartingVpnHigh, sizeof(UCHAR), file);
    readPhysicalMemory(vadRootPhysAddr + 0x21, &EndingVpnHigh, sizeof(UCHAR), file);

    std::cout << "Starting VPN: " << std::hex << StartingVpn << " Ending VPN: " << EndingVpn << '\n';
    std::cout << "Starting VPN high: " << std::hex << StartingVpnHigh << " Ending VPN high: " << EndingVpnHigh << '\n';

    LONG ReferenceCount;
    readPhysicalMemory(vadRootPhysAddr + 0x24, &ReferenceCount, sizeof(LONG), file);

    std::cout << "Reference count: " << std::dec << ReferenceCount << '\n';
}
