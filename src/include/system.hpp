#ifndef C6502PP_SYSTEM_HPP
#   define C6502PP_SYSTEM_HPP
#	include "concepts.hpp"
#   include "cpu.hpp"
#   include "bus/memory.hpp"

namespace C6502PP {

/**
 * CompileTimeSystem<Processor, BusDevice>
 *
 * Wraps an instance of a given Processor concept and BusDevice concept such that code generator is
 * able to inine away the read/write calls to the bus implementation.
 */
template <
    template <typename, typename> typename CPUType,
    BusDevice MemoryBus,
    FetchedOpcodeObserver OpcodeObserver = PassthroughOpcodeObserver
>
struct alignas(NativeCacheLine) CompileTimeSystem {

    using CPU = CPUType<MemoryBus, OpcodeObserver>;

    alignas(NativeCacheLine) CPU oCPU;
    alignas(NativeCacheLine) MemoryBus oBus;
    alignas(NativeCacheLine) OpcodeObserver oOpcodeObserver;

    CompileTimeSystem() : oCPU(oBus, oOpcodeObserver) {
        std::printf("sizeof(CompileTimeSystem) = %zu bytes\n", sizeof(CompileTimeSystem));
    }

    CompileTimeSystem& run() {
        oCPU.run();
        return *this;
    }

    CompileTimeSystem& runFrom(Address iStart) {
        oCPU.setProgramCounter(iStart).run();
        return *this;
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
        oOpcodeObserver.dumpStats();
    }
};


template <
    template <typename, typename> typename CPUType,
    BusDevice MemoryBus,
    FetchedOpcodeObserver OpcodeObserver = PassthroughOpcodeObserver
>
struct RuntimeSystem {

    using CPU = CPUType<MemoryBus, OpcodeObserver>;

    Bus::AbstractMemory& oBus;
    OpcodeObserver oOpcodeObserver;
    CPU oCPU;

    RuntimeSystem() : oBus(*Bus::AbstractMemory::createRuntimeMemory()), oCPU(oBus, oOpcodeObserver) {
        std::printf("sizeof(RuntimeSystem) = %zu bytes\n", sizeof(RuntimeSystem));
    }

    ~RuntimeSystem() {
        Bus::AbstractMemory::destroyRuntimeMemory(&oBus);
    }

    RuntimeSystem& run() {
        oCPU.run();
        return *this;
    }

    RuntimeSystem& runFrom(Address iStart) {
        oCPU.setProgramCounter(iStart).run();
        return *this;
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

    void showStatus() const noexcept {
        oCPU.showStatus();
        oOpcodeObserver.dumpStats();
    }
};

};

#endif

