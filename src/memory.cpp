#include <cstdio>
#include <cstring>
#include "bus/memory.hpp"

namespace C6502PP::Bus {

Byte RuntimeSimpleMemory::readByte(uint16_t address) const noexcept {
    return bytes[address];
}

void RuntimeSimpleMemory::writeByte(Address address, Byte value) noexcept {
    bytes[address] = value;
}

RuntimeSimpleMemory& C6502PP::Bus::RuntimeSimpleMemory::softReset() noexcept { return *this; }
RuntimeSimpleMemory& C6502PP::Bus::RuntimeSimpleMemory::hardReset() noexcept {
    std::memset(bytes, 0, sizeof(bytes));
    return *this;
}

size_t RuntimeSimpleMemory::loadImage(char const* sImage, Address iLocation) {
    size_t iResult = 0;
    std::FILE* pImage = std::fopen(sImage, "rb");
    if (pImage) {
        iResult = std::fread(bytes, 1, (MEM_SIZE - iLocation), pImage);
        std::fclose(pImage);
    }
    return iResult;
}

}
