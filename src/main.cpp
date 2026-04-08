#include <chrono>
#include <cstdio>
#include <new>
#include "system.hpp"
//#include "bus/memory.hpp"

using namespace C6502PP;

/*
struct RuntimeSystem {
    alignas(NativeCacheLine) CPU<Bus::RuntimeAbstractDevice&> oCPU;
    Bus::RuntimeAbstractDevice& oBus;
    
    RuntimeSystem(Bus::RuntimeAbstractDevice* oBus) : oCPU(*oBus), oBus(*oBus) {
        std::printf("sizeof(RuntimeSystem) = %zu bytes\n", sizeof(RuntimeSystem));
    }
    
    RuntimeSystem& softReset() noexcept {
        oCPU.softReset();
        oBus.softReset();
        return *this;
    }

    RuntimeSystem& hardReset() noexcept {
        oCPU.hardReset();
        oBus.hardReset();
        return *this;
    }
};
*/
int main() {

    // Define a system that uses the SimpleMemory realisation of BusDevice.

    static CompileTimeSystem<Bus::SimpleMemory, MOS6502<Bus::SimpleMemory>> system;

    // Initial state.

    system.showStatus();
    int const UNROLL = 10;
    
    // Singe Operation Test
    {
        size_t const LOOPS  = 10000;
        size_t const LENGTH = 32768;
        size_t const BYTES_PER_OP = SIZE_NOP;
        Address const START  = 512;
        Byte const OPERATION = NOP;

        // Put 32K ops after zero page and stack as our test code.
        system.oBus.blockFill(START, LENGTH, OPERATION);
        system.oBus.writeByte(START + LENGTH, 0xFF); // illegal opcode exit

        // Warm the caches
        system.runFrom(START);

        // Time many runs              
        size_t iCount = LOOPS * LENGTH * UNROLL / BYTES_PER_OP;
        auto tStart = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < LOOPS; ++i) {     
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
            system.runFrom(START);
        }
        auto tElapsed = std::chrono::high_resolution_clock::now() - tStart;
        size_t iNanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(tElapsed).count();
        double fMIPS = 1.0e3 * iCount / (double)iNanoSeconds;
        printf(
            "Ran %zu 0x%02X insructions in %zd nanoseconds [%.3f MIPS]\n",
            iCount,
            (unsigned)OPERATION,
            iNanoSeconds,
            fMIPS
        );
    }

    // ROM name
    char const *sROM = "data/rom/diagnostic/6502_functional_test.bin";

    // Load up the ROM
    size_t iROMSize = system.oBus.loadImage(sROM, 0);

    size_t  const KLAUS_OP_COUNT = 30648049;
    Address const KLAUS_MAGIC    = 0x3469;  // Magic endless loop address for a successful run.
    Address const KLAUS_START    = 0x400;
    if (iROMSize) {
        printf("Loaded %s\nBeginning execution from 0x%04X\n", sROM, (unsigned)KLAUS_START);

        // cache warmup
        system.runFrom(KLAUS_START);
        size_t iCount = KLAUS_OP_COUNT * UNROLL;
        auto tStart = std::chrono::high_resolution_clock::now();

        // Time 10 runs
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);
        system.runFrom(KLAUS_START);

        auto tElapsed = std::chrono::high_resolution_clock::now() - tStart;

        size_t iNanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(tElapsed).count();

        double fMIPS = 1.0e3 * iCount / (double)iNanoSeconds;

        if (system.oCPU.getProgramCounter() == KLAUS_MAGIC) {
            printf(
                "Klaus Test Passed! Ran %zu insructions in %zd nanoseconds [%.3f MIPS]\n",
                iCount,
                iNanoSeconds,
                fMIPS
            );
        } else {
            printf(
                "Terminated at 0x%04X\n after %zu insructions",
                system.oCPU.getProgramCounter(),
                iCount
            );
        }
        system.showStatus();
    }

    return 0;
}


