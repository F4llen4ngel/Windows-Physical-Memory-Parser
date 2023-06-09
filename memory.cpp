#include "memory.h"

// Dummy check function
bool check(std::size_t position) {
    // Implement your check logic here...
    return true; // Return true for now
}

bool check(std::size_t position, const std::string &filename)
{
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cerr << "Unable to open the file: " << filename << '\n';
        return false;
    }

    // calculate start of EPROCESS
    auto kprocessOffset = position - 0x5a8; // 0x5a8 is the offset of ImageFileName within _EPROCESS structure

    // Go to the calculated offset in the file
    file.seekg(kprocessOffset, std::ios::beg);

    if (file.fail())
    {
        std::cerr << "Failed to seek to the _KPROCESS position in the file\n";
        return false;
    }

    _KPROCESS kprocess;
    file.read(reinterpret_cast<char*>(&kprocess), sizeof(_KPROCESS));

    if (file.fail())
    {
        std::cerr << "Failed to read the _KPROCESS from the file\n";
        return false;
    }

    // Validate the DirectoryTableBase
    std::cout << "[DEBUG] DirectoryTableBase: " << std::hex << kprocess.DirectoryTableBase << '\n';
    if (kprocess.DirectoryTableBase == 0x1ae000)
    {
        return true;
    }

    return false;
}



std::ptrdiff_t find_system_process_address(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cerr << "Unable to open the file: " << filename << '\n';
        return -1;
    }

    std::string data(std::istreambuf_iterator<char>(file), {});

    std::size_t position = data.find("System");
    while (position != std::string::npos) // while "System" is found
    {
        std::cout << "[DEBUG] Checking 'System' at position: " << std::hex << position << '\n';
        if (check(position, filename))
        {
            return static_cast<std::ptrdiff_t>(position - 0x5a8);
        }

        // Search for next occurrence of "System"
        position = data.find("System", position + 1);
    }

    // if reached here, no valid EPROCESS found
    std::cerr << "'System' EPROCESS not found\n";
    return -1;
}

