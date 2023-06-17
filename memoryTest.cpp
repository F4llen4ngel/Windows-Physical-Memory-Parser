#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "memory.h"


#define TEST_FILE "../2.raw"


TEST_CASE("Test findSystemKProcessAddress")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    std::ptrdiff_t address = findSystemKProcessAddress(file);
    REQUIRE_EQ(address, 0x25d80178);
}

TEST_CASE("Test readPhysicalMemory")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    std::ptrdiff_t address = 0x25d80178;
    uint64_t directoryTableBase;
    readPhysicalMemory(address + DIRECTORY_TABLE_BASE, &directoryTableBase, sizeof(uint64_t), file);
    REQUIRE_EQ(directoryTableBase, _CR3);
}

TEST_CASE("Test readPhysicalMemory (bad address fail)")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t directoryTableBase;
    readPhysicalMemory(0, &directoryTableBase, sizeof(uint64_t), file);
    REQUIRE_EQ(directoryTableBase, 0);
}

TEST_CASE("Test virtualToPhysicalAddress")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t virtualAddress = 0xffffd58612045508;
    uint64_t physicalAddress = virtualToPhysicalAddress(virtualAddress, 0x6c905000, file);
    REQUIRE_EQ(physicalAddress, 0x38190508);
}

TEST_CASE("Test virtualToPhysicalAddress (bad address fail)")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t virtualAddress = 0xdeadbeef0;
    uint64_t physicalAddress = virtualToPhysicalAddress(virtualAddress, _CR3, file);
    REQUIRE_EQ(physicalAddress, 0);
}

TEST_CASE ("Test getNextProcessKProcess")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t kProcess = 0x7c733080;
    uint64_t dtb = 0xc667000;
    uint64_t nextKProcess = getNextProcessKProcess(kProcess, dtb, file);
    REQUIRE_EQ(nextKProcess, 0xc21b040);
}

TEST_CASE("Test getProcessName")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t kProcess = 0x7c733080;
    std::string processName = getProcessName(kProcess, file);
    REQUIRE_EQ(processName, "Registry");
}

TEST_CASE("Test getVadRootPhysicalAddress")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t kProcess = 0x6eba6080;
    uint64_t dtb = 0x6c905000;
    uint64_t vadRoot = getVadRootPhysicalAddress(kProcess, dtb, file);
    REQUIRE_EQ(vadRoot, 0x55d86610);
}

TEST_CASE("Test getLeftNodePhysicalAddress")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t kProcess = 0x6eba6080;
    uint64_t dtb = 0x6c905000;
    uint64_t vadRoot = getVadRootPhysicalAddress(kProcess, dtb, file);
    uint64_t leftNode = getLeftNodePhysicalAddress(vadRoot, dtb, file);
    REQUIRE_EQ(leftNode, 0x71f01960);
}

TEST_CASE("Test getRightNodePhysicalAddress")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t kProcess = 0x6eba6080;
    uint64_t dtb = 0x6c905000;
    uint64_t vadRoot = getVadRootPhysicalAddress(kProcess, dtb, file);
    uint64_t rightNode = getRightNodePhysicalAddress(vadRoot, dtb, file);
    REQUIRE_EQ(rightNode, 0x55d86660);
}

TEST_CASE("Test getParentNodePhysicalAddress")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t kProcess = 0x6eba6080;
    uint64_t dtb = 0x6c905000;
    uint64_t vadRoot = getVadRootPhysicalAddress(kProcess, dtb, file);
    uint64_t rightNode = getRightNodePhysicalAddress(vadRoot, dtb, file);
    uint64_t parentNode = getParentNodePhysicalAddress(rightNode, dtb, file);
    REQUIRE_EQ(parentNode, vadRoot);
}

TEST_CASE("Test readVadNode")
{
    std::ifstream file(TEST_FILE, std::ios::binary);
    uint64_t vadRoot = 0x55d86610;
    uint64_t dtb = 0x6c905000;
    VadNode node = readVadNode(vadRoot, dtb, file);
    REQUIRE_EQ(node.startAddress, 0x1b0b94a0000);
    REQUIRE_EQ(node.endAddress, 0x1b0b94a2000);
}

