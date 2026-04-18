#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include "system_type.hpp"

using namespace C6502PP;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <bin_file> <instr_per_op> <seconds>" << std::endl;
        return 1;
    }

    const char* binFile = argv[1];
    size_t instrPerOp = std::stoull(argv[2]);
    int seconds = std::stoi(argv[3]);

    static SystemType system;

    // Load binary at 0x0600 (default for Go benchmarks)
    if (system.oBus.loadImage(binFile, 0x0600) == 0) {
        std::cerr << "Failed to load binary " << binFile << std::endl;
        return 1;
    }

    size_t totalOps = 0;
    auto tStart = std::chrono::high_resolution_clock::now();
    auto tDeadline = tStart + std::chrono::seconds(seconds);
    
    // Benchmark Loop: Reset and run for the specified time.
    // runFrom() executes until the JAM/illegal-op exit, so a second run()
    // would re-dispatch the terminating opcode and inflate wall-clock time
    // without increasing totalOps, understating MIPS.
    while (std::chrono::high_resolution_clock::now() < tDeadline) {
        system.runFrom(0x0600);
        totalOps++;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    size_t iNanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(tEnd - tStart).count();
    
    size_t totalInstrs = totalOps * instrPerOp;
    double fMIPS = 1.0e3 * totalInstrs / (double)iNanoSeconds;
    
    std::cout << binFile << ": Ran " << totalInstrs << " instructions in " << (double)iNanoSeconds / 1e9 
              << " seconds [" << fMIPS << " MIPS] (" << totalOps << " ops)" << std::endl;

    return 0;
}
