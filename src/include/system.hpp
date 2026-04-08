#ifndef C6502PP_SYSTEM_HPP
#   define C6502PP_SYSTEM_HPP
#	include "concepts.hpp"
#   include "cpu.hpp"
#   include "bus/memory.hpp"

namespace C6502PP {

template <BusDevice MemoryBus, Processor CPU>
struct alignas(NativeCacheLine) CompileTimeSystem {
    alignas(NativeCacheLine) CPU oCPU;
    alignas(NativeCacheLine) MemoryBus oBus;

    CompileTimeSystem() : oCPU(oBus) {
        std::printf("sizeof(CompileTimeSystem) = %zu bytes\n", sizeof(CompileTimeSystem));
    }

    size_t run() {
        return oCPU.run();
    }

    size_t runFrom(Address iStart) {
        return oCPU.setProgramCounter(iStart).run();
    }

    CompileTimeSystem& softReset() noexcept {
        oCPU.softReset();
        oBus.softReset();
        return *this;
    }

    CompileTimeSystem& hardReset() noexcept {
        oCPU.hardReset();
        oBus.hardReset();
        return *this;
    }

    void showStatus() const noexcept {
        oCPU.showStatus();
    }
};

};

#endif

