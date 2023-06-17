#include  "memory.h"


/**
 * Validate _KPROCESS structure by checking if System's DirectoryTableBase is equal to _CR3.
 *
 * @param kProcessAddress: offset of _KPROCESS structure from the beginning of the file
 * @param file: file stream
 * @return: true if _KPROCESS is valid, false otherwise
 */
bool validateKProcess(uint64_t kProcessAddress, std::ifstream &file)
{
    _KPROCESS kProcess;
    readPhysicalMemory(kProcessAddress, &kProcess, sizeof(_KPROCESS), file);

    if (kProcess.DirectoryTableBase == _CR3) {
        return true;
    }

    return false;
}

/**
 * Find the offset of _KPROCESS structure of System process.
 *
 * @param file: file stream
 * @return: offset of _KPROCESS structure of System process
 */
std::ptrdiff_t findSystemKProcessAddress(std::ifstream &file)
{
    std::string data(std::istreambuf_iterator<char>(file), {});

    std::size_t position = data.find("System");
    while (position != std::string::npos) {
        auto kProcessOffset = position - IMAGE_FILE_NAME;

        if (validateKProcess(kProcessOffset, file)) {
            return static_cast<std::ptrdiff_t>(position - IMAGE_FILE_NAME);
        }

        position = data.find("System", position + 1);
    }

    std::cerr << "'System' _EPROCESS not found\n";
    return 0;
}


/**
 * Read physical memory from the file.
 *
 * @param physicalAddress: physical address to read from
 * @param buffer: buffer to store the read data
 * @param size: size of the buffer
 * @param file: file stream
 * @return: true if the physicalAddress was read successfully, false otherwise
 */
bool readPhysicalMemory(uint64_t physicalAddress, void *buffer, size_t size, std::ifstream &file)
{

    file.seekg(physicalAddress, std::ios::beg);

    if (file.fail()) {
        std::cerr << "Failed to seek to the physicalAddress position in the file\n";
        file.clear();
        return false;
    }

    file.read(reinterpret_cast<char *>(buffer), size);

    if (file.fail()) {
        std::cerr << "Failed to read the physicalAddress from the file\n";
        file.clear();
        return false;
    }

    return true;
}


/**
 * Convert virtual address to physical address.
 *
 * @param VirtualAddress: virtual address to convert
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: physical address
 */
uint64_t virtualToPhysicalAddress(uint64_t VirtualAddress, uint64_t DirectoryTableBase, std::ifstream &file)
{
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

/**
 * Get the offset of _KPROCESS structure of the next process in ActiveProcessLinks.
 * @param kProcessAddress: offset of _KPROCESS structure of the current process
 * @param DirectoryTableBase: DirectoryTableBase of the current process
 * @param file: file stream
 * @return: offset of _KPROCESS structure of the next process
 */
uint64_t getNextProcessKProcess(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream &file)
{
    uint64_t flinkVirtAddr;
    readPhysicalMemory(kProcessAddress + ACTIVE_PROCESS_LINKS_FLINK, &flinkVirtAddr, sizeof(uint64_t), file);
    uint64_t flinkPhysAddr = virtualToPhysicalAddress(flinkVirtAddr, DirectoryTableBase, file);

    uint64_t nextProcessKProcess = flinkPhysAddr - ACTIVE_PROCESS_LINKS_FLINK;

    return nextProcessKProcess;
}

/**
 * Get the offset of _KPROCESS structure of the previous process in ActiveProcessLinks.
 * @param kProcessAddress: offset of _KPROCESS structure of the current process
 * @param DirectoryTableBase: DirectoryTableBase of the current process
 * @param file: file stream
 * @return: offset of _KPROCESS structure of the previous process
 */
uint64_t getPreviousProcessKProcess(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream &file)
{
    uint64_t blinkVirtAddr;
    readPhysicalMemory(kProcessAddress + ACTIVE_PROCESS_LINKS_BLINK, &blinkVirtAddr, sizeof(uint64_t), file);
    uint64_t blinkPhysAddr = virtualToPhysicalAddress(blinkVirtAddr, DirectoryTableBase, file);

    uint64_t previousProcessKProcess = blinkPhysAddr - ACTIVE_PROCESS_LINKS_FLINK;

    return previousProcessKProcess;
}

/**
 * Get the name of the process.
 * @param kProcessAddress: offset of _KPROCESS structure of the process
 * @param file: file stream
 * @return: name of the process
 */
std::string getProcessName(uint64_t kProcessAddress, std::ifstream &file)
{
    char processName[15];
    readPhysicalMemory(kProcessAddress + IMAGE_FILE_NAME, processName, 15, file);

    std::string processNameStr{processName};

    if (processNameStr.empty()) {
        processNameStr = "###";
    }

    return processNameStr;
}

/**
 * Get the list of processes.
 * @param systemKProcessAddress: offset of _KPROCESS structure of the system process
 * @param systemDirectoryTableBase: DirectoryTableBase of the system process
 * @param file: file stream
 * @return: list of processes
 */
std::vector<Process> getProcessList(uint64_t systemKProcessAddress, uint64_t systemDirectoryTableBase, std::ifstream &file)
{
    std::vector<Process> processList;

    uint64_t curProcessKProcess = systemKProcessAddress;
    std::string curProcessName = getProcessName(curProcessKProcess, file);
    uint64_t curProcessDirectoryTableBase = systemDirectoryTableBase;
    std::vector<VadNode> curVadTree = readProcessVadTree(curProcessKProcess, curProcessDirectoryTableBase, file);

    Process curProcess{curProcessKProcess,
                       curProcessDirectoryTableBase,
                       curProcessName,
                       curVadTree};
    processList.push_back(curProcess);

    do {
        curProcessKProcess = getNextProcessKProcess(curProcessKProcess, systemDirectoryTableBase, file);
        curProcessName = getProcessName(curProcessKProcess, file);
        readPhysicalMemory(curProcessKProcess + DIRECTORY_TABLE_BASE, &curProcessDirectoryTableBase, sizeof(uint64_t), file);
        curVadTree = readProcessVadTree(curProcessKProcess, curProcessDirectoryTableBase, file);

        curProcess = Process{curProcessKProcess,
                           curProcessDirectoryTableBase,
                           curProcessName,
                             curVadTree};
        processList.push_back(curProcess);

    } while (curProcessDirectoryTableBase != systemDirectoryTableBase);

    return processList;
}

/**
 * Get the offset of _RTL_AVL_TREE structure of the process.
 * @param kProcessPhysAddr: offset of _KPROCESS structure of the process
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: offset of _RTL_AVL_TREE structure of the process
 */
uint64_t getVadRootPhysicalAddress(uint64_t kProcessPhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t vadRootVirtAddr;
    readPhysicalMemory(kProcessPhysAddr + VAD_ROOT, &vadRootVirtAddr, sizeof(uint64_t), file);
    uint64_t vadRootPhysAddr = virtualToPhysicalAddress(vadRootVirtAddr, DirectoryTableBase, file);

    return vadRootPhysAddr;
}

/**
 * Get the offset of _RTL_BALANCED_NODE structure of the left node.
 * @param nodePhysAddr: offset of _RTL_BALANCED_NODE structure of the current node
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: offset of _RTL_BALANCED_NODE structure of the left node
 */
uint64_t getLeftNodePhysicalAddress(uint64_t nodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t leftVirtAddr;
    readPhysicalMemory(nodePhysAddr, &leftVirtAddr, sizeof(uint64_t), file);

    if (leftVirtAddr == 0x0)
    {
        return 0;
    }

    uint64_t leftPhysAddr = virtualToPhysicalAddress(leftVirtAddr, DirectoryTableBase, file);

    return leftPhysAddr;
}

/**
 * Get the offset of _RTL_BALANCED_NODE structure of the right node.
 * @param nodePhysAddr: offset of _RTL_BALANCED_NODE structure of the current node
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: offset of _RTL_BALANCED_NODE structure of the right node
 */
uint64_t getRightNodePhysicalAddress(uint64_t nodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t rightVirtAddr;
    readPhysicalMemory(nodePhysAddr + RIGHT_CHILD, &rightVirtAddr, sizeof(uint64_t), file);

    if (rightVirtAddr == 0x0)
    {
        return 0;
    }

    uint64_t rightPhysAddr = virtualToPhysicalAddress(rightVirtAddr, DirectoryTableBase, file);

    return rightPhysAddr;
}

/**
 * Get the offset of _RTL_BALANCED_NODE structure of the parent node.
 * @param nodePhysAddr: offset of _RTL_BALANCED_NODE structure of the current node
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: offset of _RTL_BALANCED_NODE structure of the parent node
 */
uint64_t getParentNodePhysicalAddress(uint64_t nodePhysAddr, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t parentValueVirtAddr;
    readPhysicalMemory(nodePhysAddr + PARENT_VALUE, &parentValueVirtAddr, sizeof(uint64_t), file);
    parentValueVirtAddr = parentValueVirtAddr & (~0x7);
    uint64_t parentValuePhysAddr = virtualToPhysicalAddress(parentValueVirtAddr, DirectoryTableBase, file);

    return parentValuePhysAddr;
}

/**
 * Read the _MMVAD_SHORT structure of the node and calculate the start and end of the page assigned to node.
 * @param nodePhysicalAddress: offset of _MMVAD_SHORT structure of the node
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: VadNode structure containing the start and end of the page assigned to node
 */
VadNode readVadNode(uint64_t nodePhysicalAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    ULONG StartingVpn, EndingVpn;
    UCHAR StartingVpnHigh, EndingVpnHigh;

    readPhysicalMemory(nodePhysicalAddress + STARTING_VPN, &StartingVpn, sizeof(ULONG), file);
    readPhysicalMemory(nodePhysicalAddress + ENDING_VPN, &EndingVpn, sizeof(ULONG), file);
    readPhysicalMemory(nodePhysicalAddress + STARTING_VPN_HIGH, &StartingVpnHigh, sizeof(UCHAR), file);
    readPhysicalMemory(nodePhysicalAddress + ENDING_VPN_HIGH, &EndingVpnHigh, sizeof(UCHAR), file);

    uint64_t start = ((uint64_t)StartingVpn << 12) | ((uint64_t)StartingVpnHigh << 44);
    uint64_t end = (((uint64_t)EndingVpn + 1ll) << 12) | ((uint64_t)EndingVpnHigh << 44);

    return VadNode{start, end};
}

/**
 * Read all nodes in the _RTL_AVL_TREE structure of the process.
 * @param nodePhysicalAddress: offset of _MMVAD_SHORT structure of the root node
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: vector of VadNode structures
 */
std::vector<VadNode> readVadTree(uint64_t nodePhysicalAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t leftNode = getLeftNodePhysicalAddress(nodePhysicalAddress, DirectoryTableBase, file);
    uint64_t rightNode = getRightNodePhysicalAddress(nodePhysicalAddress, DirectoryTableBase, file);

    std::vector<VadNode> nodes;

    if (leftNode != 0x0)
    {
        std::vector<VadNode> leftNodes = readVadTree(leftNode, DirectoryTableBase, file);
        nodes.insert(nodes.end(), leftNodes.begin(), leftNodes.end());
    }

    VadNode node = readVadNode(nodePhysicalAddress, DirectoryTableBase, file);
    nodes.push_back(node);

    if (rightNode != 0x0)
    {
        std::vector<VadNode> rightNodes = readVadTree(rightNode, DirectoryTableBase, file);
        nodes.insert(nodes.end(), rightNodes.begin(), rightNodes.end());
    }

    return nodes;
}

/**
 * Read all nodes in the _RTL_AVL_TREE structure of the process.
 * @param kProcessPhysicalAddress: offset of _EPROCESS structure of the process
 * @param DirectoryTableBase: DirectoryTableBase of the process
 * @param file: file stream
 * @return: vector of VadNode structures
 */
std::vector<VadNode> readProcessVadTree(uint64_t kProcessPhysicalAddress, uint64_t DirectoryTableBase, std::ifstream& file)
{
    uint64_t vadRoot = getVadRootPhysicalAddress(kProcessPhysicalAddress, DirectoryTableBase, file);
    return readVadTree(vadRoot, DirectoryTableBase, file);
}