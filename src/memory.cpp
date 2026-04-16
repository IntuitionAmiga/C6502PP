#include <cstdio>
#include <cstring>
#include "bus/memory.hpp"
#include <new>

namespace C6502PP::Bus {

// Methods not immediately inline
class RuntimeSimpleMemory : public AbstractMemory {

private:
    Byte aBytes[MEM_SIZE] = { 0 };

public:
    ~RuntimeSimpleMemory() {}
    Byte readByte(Address const address) const noexcept;
    void writeByte(Address const address, Byte value) noexcept;
    RuntimeSimpleMemory& softReset() noexcept;
    RuntimeSimpleMemory& hardReset() noexcept;
    RuntimeSimpleMemory& blockFill(Address iFrom, Word iLength, Byte iValue) noexcept;
    size_t loadImage(char const* sImage, Address iLocation = 0);
};


Byte RuntimeSimpleMemory::readByte(Address const address) const noexcept {
    return aBytes[address];
}

void RuntimeSimpleMemory::writeByte(Address const address, Byte const value) noexcept {
    aBytes[address] = value;
}

RuntimeSimpleMemory& RuntimeSimpleMemory::softReset() noexcept { return *this; }
RuntimeSimpleMemory& RuntimeSimpleMemory::hardReset() noexcept {
    std::memset(aBytes, 0, MEM_SIZE);
    return *this;
}

RuntimeSimpleMemory& RuntimeSimpleMemory::blockFill(Address iFrom, Word iLength, Byte iValue) noexcept {
    Word iMaxLength = MEM_SIZE - iFrom;
    iLength = (iMaxLength > iLength) ? iMaxLength : iLength;
    if (iLength) {
        std::memset(&aBytes[iFrom], iValue, iLength);
    }
    return *this;
}

size_t RuntimeSimpleMemory::loadImage(char const* sImage, Address iLocation) {
    size_t iResult = 0;
    std::FILE* pImage = std::fopen(sImage, "rb");
    if (pImage) {
        iResult = std::fread(&aBytes[iLocation], 1, (MEM_SIZE - iLocation), pImage);
        std::fclose(pImage);
    }
    return iResult;
}

AbstractMemory* AbstractMemory::createRuntimeMemory()
{
    return new RuntimeSimpleMemory;
}

void AbstractMemory::destroyRuntimeMemory(AbstractMemory* pMemory)
{
    if (pMemory) {
        delete pMemory;
    }
}

}
