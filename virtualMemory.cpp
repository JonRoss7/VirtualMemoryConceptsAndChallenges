#include "virtualMemory.h"
#include <iostream>
#include <algorithm>


VirtualMemory::VirtualMemory(int tlbSize, int pageCount) {
    this->tlbSize = tlbSize;
    this->currentTime = 0;

    tlb.reserve(tlbSize); 
}

bool VirtualMemory::lookupTLB(int pid, int virtualAddress, int& physicalAddress) {
    currentTime++;
    
    
    int pageNumber = virtualAddress >> 12;
    int offset = virtualAddress & 0xFFF; 

    
    for (auto& entry : tlb) {
        if (entry.pid == pid && entry.pageNumber == pageNumber) {
            entry.lastUsed = currentTime; 
            physicalAddress = (entry.frameNumber << 12) | offset;
            return true;
        }
    }

    
    return false;
}


bool VirtualMemory::translateAddress(int pid, int virtualAddress, int& physicalAddress) {
    
    int offset = virtualAddress & 0xFFF;
    int pageNumber = virtualAddress >> 12;       
    int outerIndex = pageNumber >> 10;          
    int innerIndex = pageNumber & 0x3FF;         

    if (outerPageTable.find({pid, outerIndex}) == outerPageTable.end()) {
        return false; 
    }
    
    if (innerPageTable.find({pid, pageNumber}) == innerPageTable.end()) {
        return false; 
    }

    int frameNumber = innerPageTable[{pid, pageNumber}];
    physicalAddress = (frameNumber << 12) | offset;
    
    return true;
}

void VirtualMemory::insertTLB(int pid, int pageNumber, int frameNumber) {
    for (auto& entry : tlb) {
        if (entry.pid == pid && entry.pageNumber == pageNumber) {
            entry.frameNumber = frameNumber;
            entry.lastUsed = currentTime;
            return;
        }
    }

    TLBEntry newEntry;
    newEntry.pid = pid;
    newEntry.pageNumber = pageNumber;
    newEntry.frameNumber = frameNumber;
    newEntry.lastUsed = currentTime;

    if (tlb.size() < tlbSize) {
        tlb.push_back(newEntry);
    } else {
        auto lruEntry = std::min_element(tlb.begin(), tlb.end(), 
            [](const TLBEntry& a, const TLBEntry& b) {
                return a.lastUsed < b.lastUsed;
            });
        
        *lruEntry = newEntry;
    }
}

void VirtualMemory::loadPage(int pid, int pageNumber, int frameNumber) {
    int outerIndex = pageNumber >> 10;

    outerPageTable[{pid, outerIndex}] = 1; 

    innerPageTable[{pid, pageNumber}] = frameNumber;
}

void VirtualMemory::printStats() const {
    std::cout << "Simulation finished." << std::endl;
}