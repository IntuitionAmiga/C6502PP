#ifndef C6502PP_HPP
#	define C6502PP_HPP
#	include <cstdint>
#   include <cstdio>
#	include "concepts.hpp"
#	include "constants.hpp"

namespace C6502PP {

    /**
     * MOS6502
     */
    template <BusDevice Bus, FetchedOpcodeObserver OpcodeObserver = PassthroughOpcodeObserver>
    class MOS6502 {

    private:
        static_assert(0 == (STACK_BASE & 0xFF), "STACK_BASE is not page aligned");
        static_assert((STACK_TOP > STACK_BASE), "STACK_TOP must be higher than STACK_BASE");

        Bus &oOutside;

        OpcodeObserver& oOpcodeObserver;

        Address iProgramCounter = {0};
        Byte iAccumulator = {0};
        Byte iStatus = {F_ZERO};
        Byte iXIndex = {0};
        Byte iYIndex = {0};
        Byte iStackPointer = {STACK_TOP - STACK_BASE};

    public:
        MOS6502(Bus& oBus, OpcodeObserver& oOpcodeObserver):
            oOutside(oBus),
            oOpcodeObserver(oOpcodeObserver)
        {
            // Does not call reset() since we are not allowed to access the bus here.
        }

        void reset() noexcept {
            iAccumulator    = 0;
            iXIndex         = 0;
            iYIndex         = 0;
            iStackPointer   = STACK_TOP - STACK_BASE; // Offset in the page at STACK_BASE
            iProgramCounter = readWord(VEC_RES);      // Load from reset vector
            iStatus         = F_ZERO;
        }

        MOS6502& softReset() noexcept {
            reset();
            return *this;
        }

        MOS6502& hardReset() noexcept {
            reset();
            return *this;
        }

        Address getProgramCounter() const noexcept {
            return iProgramCounter;
        }

        MOS6502& setProgramCounter(Address iAddress) noexcept {
            iProgramCounter = iAddress;
            return *this;
        }

        void showStatus() const noexcept {
            std::printf(
                "PC: 0x%04X => 0x%02X\n"
                "SP: 0x%04X => 0x%02X\n"
                "A: 0x%02X [%4d]\n"
                "X: 0x%02X [%4d]\n"
                "Y: 0x%02X [%4d]\n"
                "F: [%c %c | %c %c %c %c %c]\n",
                (unsigned)iProgramCounter,
                (unsigned)oOutside.readByte(iProgramCounter),
                (unsigned)(STACK_BASE + iStackPointer),
                (unsigned)oOutside.readByte(STACK_BASE + iStackPointer),
                (unsigned)iAccumulator,
                (int)(int8_t)iAccumulator,
                (unsigned)iXIndex,
                (int)(int8_t)iXIndex,
                (unsigned)iYIndex,
                (int)(int8_t)iYIndex,
                (iStatus & F_NEGATIVE ? 'N' : '-'),
                (iStatus & F_OVERFLOW ? 'V' : '-'),
                (iStatus & F_BREAK ? 'B' : '-'),
                (iStatus & F_DECIMAL ? 'D' : '-'),
                (iStatus & F_INTERRUPT ? 'I' : '-'),
                (iStatus & F_ZERO ? 'Z' : '-'),
                (iStatus & F_CARRY ? 'C' : '-')
            );
        }

    private:

        inline static void updateNZ(Byte& iStatus, Byte const iValue) noexcept {
            iStatus &= F_CLR_NZ;
            iStatus |= iValue ? (iValue & F_NEGATIVE) : F_ZERO;
        }


#   define _INTERNALS_INCLUDES_
#   include "internals/addressing.hpp"
#   include "internals/alu.hpp"

    public:
#   include "internals/run.hpp"
#   undef _INTERNALS_INCLUDES_

    };


}

#endif
