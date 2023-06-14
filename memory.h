//
// Created by vanya on 6/9/2023.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "structs.h"

#ifndef DUDEDUMPER_MEMORY_H
#define DUDEDUMPER_MEMORY_H

bool validateKProcess(uint64_t kProcessAddress, std::ifstream& file);
std::ptrdiff_t findSystemKProcessAddress(std::ifstream& file);
uint64_t virtualToPhysicalAddress(uint64_t VirtualAddress, uint64_t DirectoryTableBase, std::ifstream& file);
void printNextProcessName(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream& file);
bool readPhysicalMemory(uint64_t physicalAddress, void* buffer, size_t size, std::ifstream& file);
uint64_t getNextProcessKProcess(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream& file);
uint64_t getPreviousProcessKProcess(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream& file);
std::string getProcessName(uint64_t kProcessAddress, std::ifstream& file);
std::vector<Process> getProcessList(uint64_t systemKProcessAddress, uint64_t systemDirectoryTableBase, std::ifstream &file);
void getProcessMemorySpace(uint64_t kProcessAddress, uint64_t DirectoryTableBase, std::ifstream& file);

#endif //DUDEDUMPER_MEMORY_H
