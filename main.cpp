#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "virtualMemory.h"
#include "clock.h"

const int TLB_SIZE = 8;          
const int NUM_FRAMES = 16;       
const int PAGE_COUNT = 1 << 20;

int main() {
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input.txt. Make sure the file exists." << std::endl;
        return 1;
    }

    VirtualMemory vm(TLB_SIZE, PAGE_COUNT);
    Clock clockAlgo(NUM_FRAMES);

    int totalAccesses = 0;
    int tlbHits = 0;
    int tlbMisses = 0;
    int pageFaults = 0;

    int pid;
    std::string hexAddressStr;

    std::cout << "Starting Virtual Memory Simulation..." << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    std::cout << "| " << std::left << std::setw(4) << "PID" 
              << "| " << std::setw(12) << "Virtual Addr" 
              << "| " << std::setw(12) << "Phys Addr" 
              << "| " << std::setw(35) << "Status" << " |" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    while (inputFile >> pid >> hexAddressStr) {
        totalAccesses++;

        unsigned int virtualAddr = std::stoul(hexAddressStr, nullptr, 16);
        
        int pageNumber = virtualAddr >> 12;
        int offset = virtualAddr & 0xFFF;
        
        int physicalAddr = 0;
        std::string statusMsg = "";

        bool tlbHit = vm.lookupTLB(pid, virtualAddr, physicalAddr);

        if (tlbHit) {
            tlbHits++;
            statusMsg = "TLB Hit";
            
        } 
        else {
            tlbMisses++;

            bool ptHit = vm.translateAddress(pid, virtualAddr, physicalAddr);

            if (ptHit) {
                statusMsg = "TLB Miss, Page Table Hit";
                
                int frameNum = physicalAddr >> 12;
                vm.insertTLB(pid, pageNumber, frameNum);
            } 
            else {
                pageFaults++;
                statusMsg = "Page Fault";

                int evictedPage = -1;
                
                int frameNum = clockAlgo.evictAndReplace(pid, pageNumber, evictedPage);

                if (evictedPage != -1) {
                    statusMsg += " [Evicted Pg " + std::to_string(evictedPage) + "]";
                }

                vm.loadPage(pid, pageNumber, frameNum);

                vm.insertTLB(pid, pageNumber, frameNum);

                physicalAddr = (frameNum << 12) | offset;
            }
        }

        std::cout << "| " << std::left << std::setw(4) << pid 
                  << "| 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << virtualAddr << std::dec << std::setfill(' ')
                  << "  | 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << physicalAddr << std::dec << std::setfill(' ')
                  << "  | " << std::setw(35) << statusMsg << " |" << std::endl;
    }

    std::cout << std::string(75, '-') << std::endl;
    std::cout << "\nSimulation Results Summary" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "Total Memory Accesses: " << totalAccesses << std::endl;
    
    double hitRate = (totalAccesses > 0) ? ((double)tlbHits / totalAccesses) * 100.0 : 0.0;
    std::cout << "TLB Hits:              " << tlbHits << " (" << std::fixed << std::setprecision(2) << hitRate << "%)" << std::endl;
    
    std::cout << "TLB Misses:            " << tlbMisses << std::endl;
    
    double faultRate = (totalAccesses > 0) ? ((double)pageFaults / totalAccesses) * 100.0 : 0.0;
    std::cout << "Page Faults:           " << pageFaults << " (" << std::fixed << std::setprecision(2) << faultRate << "%)" << std::endl;

    return 0;
}