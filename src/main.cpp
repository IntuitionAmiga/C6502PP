#include <chrono>
#include <cstdio>
#include <new>
#include "cpu.hpp"
#include "bus/memory.hpp"

using namespace C6502PP;

template <BusDevice MemoryBus>
struct alignas(NativeCacheLine) TestSystem {
    alignas(NativeCacheLine) CPU<MemoryBus> oCPU;
    alignas(NativeCacheLine) MemoryBus oBus;

    TestSystem() : oCPU(oBus) {
        std::printf("sizeof(TestSystem) = %zu bytes\n", sizeof(TestSystem));
    }

    TestSystem& softReset() noexcept {
        oCPU.softReset();
        oBus.softReset();
        return *this;
    }

    TestSystem& hardReset() noexcept {
        oCPU.hardReset();
        oBus.hardReset();
        return *this;
    }

    void showStatus() const noexcept {
        std::printf(
            "PC: 0x%04X, points to %p\n"
            "SP: 0x%02X [0x%04X], points to %p\n"
            "A: 0x%02X [%d]\n"
            "X: 0x%02X [%d]\n"
            "Y: 0x%02X [%d]\n"
            "F: %c %c | %c %c %c %c %c\n",
            (unsigned)oCPU.iProgramCounter,
            &oBus.bytes[oCPU.iProgramCounter],
            (unsigned)oCPU.iStackPointer,
            (unsigned)(STACK_BASE + oCPU.iStackPointer),
            &oBus.bytes[STACK_BASE + oCPU.iStackPointer],
            (unsigned)oCPU.iAccumulator,
            (int)(int8_t)oCPU.iAccumulator,
            (unsigned)oCPU.iXIndex,
            (int)(int8_t)oCPU.iXIndex,
            (unsigned)oCPU.iYIndex,
            (int)(int8_t)oCPU.iYIndex,
            (int)(oCPU.iStatus & F_NEGATIVE ? 'N' : '-'),
            (int)(oCPU.iStatus & F_OVERFLOW ? 'V' : '-'),
            (int)(oCPU.iStatus & F_BREAK ? 'B' : '-'),
            (int)(oCPU.iStatus & F_DECIMAL ? 'D' : '-'),
            (int)(oCPU.iStatus & F_INTERRUPT ? 'I' : '-'),
            (int)(oCPU.iStatus & F_ZERO ? 'Z' : '-'),
            (int)(oCPU.iStatus & F_CARRY ? 'C' : '-')
        );
    }
};

int main() {
    static TestSystem<Bus::SimpleMemory> system;

    // Initial state.
    system.showStatus();

    char const *sROM = "data/rom/diagnostic/6502_functional_test.bin";

    Address const KLAUS_MAGIC = 0x3469;

    std::FILE* pROM = fopen(sROM, "rb");
    if (pROM) {
        fread(system.oBus.bytes, 1, 65536, pROM);
        fclose(pROM);
        printf("Loaded %s\n", sROM);
        system.oCPU.iProgramCounter = 0x400;
        printf("Beginning execution from $%04X\n", system.oCPU.iProgramCounter);

        // cache warmup
        system.oCPU.run();
        system.oCPU.iProgramCounter = 0x400;

        auto tStart = std::chrono::high_resolution_clock::now();
        size_t iCount = system.oCPU.run();
        auto tElapsed = std::chrono::high_resolution_clock::now() - tStart;

        size_t iNanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(tElapsed).count();

        double fMIPS = 1.0e3 * iCount / (double)iNanoSeconds;

        if (system.oCPU.iProgramCounter == KLAUS_MAGIC) {
            printf(
                "Klaus Test Passed! Ran %zu insructions in %zd nanoseconds [%.3f MIPS]\n",
                iCount,
                iNanoSeconds,
                fMIPS
            );
        } else {
            printf(
                "Terminated at $%04X\n after %zu insructions",
                system.oCPU.iProgramCounter,
                iCount
            );
        }
        system.showStatus();
    }

    return 0;
}
