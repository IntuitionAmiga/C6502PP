#ifdef _INTERNALS_INCLUDES_

    /**
     * Assorted reusable ALU bits
     */

    inline static void cmpByte(Byte& iStatus, Byte const iTo, Byte const iValue) noexcept {
        Byte iRes = iTo - iValue;

        // Deal with the result
        iStatus &= F_CLR_NZC;
        iStatus |= (iRes ? (iRes & F_NEGATIVE) : F_ZERO);
        iStatus |= (iValue > iTo) ? 0 : F_CARRY;
    }

    inline static Byte shiftLeftWithCarry(Byte& iStatus, Byte iValue) noexcept {
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_NEGATIVE) >> 7; // sign -> carry
        updateNZ(iStatus,  iValue <<= 1 );
        return iValue;
    }

    inline static Byte shiftRightWithCarry(Byte& iStatus, Byte iValue) noexcept {
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_CARRY);
        updateNZ(iStatus,  iValue >>= 1 );
        return iValue;
    }


    inline static void lsrMemory(Bus& oOutside,  Byte& iStatus, Address const iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            shiftRightWithCarry(iStatus, oOutside.readByte(iAddress))
        );
    }

    inline static void aslMemory(Bus& oOutside, Byte& iStatus, Address const iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            shiftLeftWithCarry(iStatus, oOutside.readByte(iAddress))
        );
    }

    inline static Byte rotateLeftWithCarry(Byte& iStatus, Byte iValue) noexcept {
        Byte iCarry = iStatus & F_CARRY;
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_NEGATIVE) >> 7; // sign -> carry
        updateNZ(iStatus,  iValue = ((iValue << 1) | iCarry) );
        return iValue;
    }

    inline static Byte rotateRightWithCarry(Byte& iStatus, Byte iValue) noexcept {
        uint8_t iCarry = (iStatus & F_CARRY) << 7; // carry -> sign
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_CARRY); // carry -> carry
        updateNZ(iStatus,  iValue = ( (iValue >> 1) | iCarry) );
        return iValue;
    }

    inline static void rolMemory(Bus& oOutside, Byte& iStatus, Address const iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            rotateLeftWithCarry(iStatus, oOutside.readByte(iAddress))
        );
    }

    inline static void rorMemory(Bus& oOutside, Byte& iStatus, Address const iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            rotateRightWithCarry(iStatus, oOutside.readByte(iAddress))
        );
    }

    /**
     * Add byte, carry and accumulator using BCD semantics.
     * Immediate result is allowed to overflow the normal byte range so that we can
     * use bit 8 as the overflow.
     */
    inline static unsigned addBCDWithCarry(Byte& iStatus, Byte& iAccumulator, Byte iValue) noexcept {
        // Nybbles
        unsigned iSumL = (iAccumulator & 0x0F) + (iValue & 0x0F) + (iStatus & F_CARRY);
        unsigned iSumH = (iAccumulator & 0xF0) + (iValue & 0xF0);

        if (iSumL > 0x09) {
            iSumH += 0x10; // Carry
            iSumL += 0x06; // Wrap
        }
        if (iSumH > 0x90) {
            iSumH += 0x60; // Wrap
        }
        return (iSumH & 0x1F0) | (iSumL & 0x0F);
    }

    /**
     * Subtract byte and carry from the accumulator, using BCD semantics
     * Immediate result is allowed to underflow the normal byte range so that we can
     * use bit 8 as the underflow.
     */
    inline static unsigned subBCDWithCarry(Byte& iStatus, Byte& iAccumulator, Byte iValue) noexcept {
        // Nybbles
        unsigned iDiffL = (iAccumulator & 0x0F) - (iValue & 0x0F) - (~iStatus & F_CARRY);
        unsigned iDiffH = (iAccumulator & 0xF0) - (iValue & 0xF0);

        if (iDiffL & 0x10) {
            iDiffL -= 0x06;
            --iDiffH;
        }
        if (iDiffH & 0x0100) {
            iDiffH -= 0x60;
        }
        return (iDiffH & 0x1F0) | (iDiffL & 0x0F);
    }

    inline static void addByteWithCarry(Byte& iStatus, Byte& iAccumulator, Byte iValue) noexcept {
        unsigned iSum = (iStatus & F_DECIMAL) ?
            addBCDWithCarry(iStatus, iAccumulator, iValue) :
            (iStatus & F_CARRY) + iValue + iAccumulator;

        Byte iRes = iSum; // & 0xFF truncates

        // Deal with the result
        iStatus &= F_CLR_NZCV;
        iStatus |= (iRes ? (iRes & F_NEGATIVE) : F_ZERO);
        iStatus |= (iSum & 0x100) ? F_CARRY : 0;
        iStatus |= (
            (iValue & F_NEGATIVE) == (iAccumulator & F_NEGATIVE) &&
            (iValue & F_NEGATIVE) != (iRes & F_NEGATIVE)
        ) ? F_OVERFLOW : 0;

        iAccumulator = iRes;
    }

    inline static void subByteWithCarry(Byte& iStatus, Byte& iAccumulator, Byte iValue) noexcept {
        unsigned iDiff = (iStatus & F_DECIMAL) ?
            subBCDWithCarry(iStatus, iAccumulator, iValue) :
            iAccumulator - iValue - (~iStatus & F_CARRY);

        Byte iRes = iDiff; // & 0xFF truncates

        // Deal with the result
        iStatus &= F_CLR_NZCV;
        iStatus |= (iRes ? (iRes & F_NEGATIVE) : F_ZERO);
        iStatus |= (iDiff & 0x100) ? 0 : F_CARRY;
        iStatus |= (
            (iValue & F_NEGATIVE) != (iAccumulator & F_NEGATIVE) &&
            (iAccumulator & F_NEGATIVE) != (iRes & F_NEGATIVE)
        ) ? F_OVERFLOW : 0;

        iAccumulator = iRes;
    }

#   define INTERNALS_ADDRESSING
#endif

