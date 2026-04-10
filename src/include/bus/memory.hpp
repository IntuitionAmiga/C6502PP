#ifndef C6502PP_BUS_MEMORY_HPP
#	define C6502PP_BUS_MEMORY_HPP
#	include "concepts.hpp"
#	include "constants.hpp"
#	include <cstring>
#   include <cstdio>

namespace C6502PP::Bus {

/**
 * Simple flat memory implementation of ByteAccessible concept
 */
class SimpleMemory {

private:
    Byte aBytes[MEM_SIZE] = { 0 };

public:
    Byte readByte(uint16_t iAddress) const noexcept {
        return aBytes[iAddress];
    }

    void writeByte(Address iAddress, Byte iValue) noexcept {
        aBytes[iAddress] = iValue;
    }

    SimpleMemory& blockFill(Address iFrom, Word iLength, Byte iValue) noexcept {
        Word iMaxLength = MEM_SIZE - iFrom;
        iLength = (iMaxLength > iLength) ? iMaxLength : iLength;
        if (iLength) {
            std::memset(&aBytes[iFrom], iValue, iLength);
        }
        return *this;
    }

    SimpleMemory& softReset() noexcept {
        return *this;
    }

    SimpleMemory& hardReset() noexcept {
        std::memset(aBytes, 0, MEM_SIZE);
        return *this;
    }

    /**
     * Load an image into memory at the given address. Attempts to load at most MEM_SIZE data.
     * Returns total bytes read.
     */
    size_t loadImage(char const* sImage, Address iLocation = 0) {
        size_t iResult = 0;
        std::FILE* pImage = std::fopen(sImage, "rb");
        if (pImage) {
            iResult = std::fread(&aBytes[iLocation], 1, (MEM_SIZE - iLocation), pImage);
            std::fclose(pImage);
        }
        return iResult;
    }
};

/**
 * Provide a runtime abstraction for comparison purposes.
 */
class AbstractMemory {
public:
    virtual ~AbstractMemory() {};
    virtual AbstractMemory& softReset() noexcept = 0;
    virtual AbstractMemory& hardReset() noexcept = 0;
    virtual Byte readByte(Address iAddress) const noexcept = 0;
    virtual void writeByte(Address iAddress, Byte iByte) noexcept = 0;
    virtual AbstractMemory& blockFill(Address iFrom, Word iLength, Byte iValue) noexcept = 0;
    virtual size_t loadImage(char const* sImage, Address iLocation = 0) = 0;

    // To foil LTO, we hide direct visibility of what will be created.
    static AbstractMemory* createRuntimeMemory();
    static void destroyRuntimeMemory(AbstractMemory* oMemory);
};




}

#endif
