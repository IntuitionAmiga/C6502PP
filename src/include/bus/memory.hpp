#ifndef C6502PP_BUS_MEMORY_HPP
#	define C6502PP_BUS_MEMORY_HPP
#	include "concepts.hpp"
#	include "constants.hpp"
#	include <cstring>

namespace C6502PP::Bus {

/**
 * Simple flat memory implementation of ByteAccessible concept
 */
struct SimpleMemory {
    Byte bytes[MEM_SIZE] = { 0 };

    Byte readByte(uint16_t address) const noexcept {
        return bytes[address];
    }

    void writeByte(Address address, Byte value) noexcept {
        bytes[address] = value;
    }

    SimpleMemory& softReset() noexcept { return *this; }
    SimpleMemory& hardReset() noexcept {
        std::memset(bytes, 0, sizeof(bytes));
        return *this;
    }
};

}

#endif
