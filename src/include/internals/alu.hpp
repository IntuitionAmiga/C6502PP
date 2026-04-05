#ifdef _INTERNALS_INCLUDES_

    /**
     * Assorted reusable ALU bits
     */

    void cmpByte(Byte iTo, Byte iValue) noexcept {
        Byte iRes = iTo - iValue;

        // Deal with the result
        iStatus &= F_CLR_NZC;
        iStatus |= (iRes ? (iRes & F_NEGATIVE) : F_ZERO);
        iStatus |= (iValue > iTo) ? 0 : F_CARRY;
    }


    /**
    * Pull a raw byte off the stack
    */
    Byte pullByte() noexcept {
        ++iStackPointer;
        return oOutside.readByte(iStackPointer + STACK_BASE);
    }

    /**
    * Push a byte on the stack
    */
    void pushByte(Byte iValue) noexcept {
        oOutside.writeByte(STACK_BASE + iStackPointer, iValue);
        --iStackPointer;
    }

    Byte shiftLeftWithCarry(Byte iValue) noexcept {
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_NEGATIVE) >> 7; // sign -> carry
        updateNZ( iValue <<= 1 );
        return iValue;
    }

    Byte shiftRightWithCarry(Byte iValue) noexcept {
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_CARRY);
        updateNZ( iValue >>= 1 );
        return iValue;
    }


    void lsrMemory(Address iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            shiftRightWithCarry(oOutside.readByte(iAddress))
        );
    }

    void aslMemory(Address iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            shiftLeftWithCarry(oOutside.readByte(iAddress))
        );
    }

    Byte rotateLeftWithCarry(Byte iValue) noexcept {
        Byte iCarry = iStatus & F_CARRY;
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_NEGATIVE) >> 7; // sign -> carry
        updateNZ( iValue = ((iValue << 1) | iCarry) );
        return iValue;
    }

    Byte rotateRightWithCarry(Byte iValue) noexcept {
        uint8_t iCarry = (iStatus & F_CARRY) << 7; // carry -> sign
        iStatus &= ~F_CARRY;
        iStatus |= (iValue & F_CARRY); // carry -> carry
        updateNZ( iValue = ( (iValue >> 1) | iCarry) );
        return iValue;
    }

    void rolMemory(Address iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            rotateLeftWithCarry(oOutside.readByte(iAddress))
        );
    }

    void rorMemory(Address iAddress) noexcept {
        oOutside.writeByte(
            iAddress,
            rotateRightWithCarry(oOutside.readByte(iAddress))
        );
    }

    /**
     * Add byte, carry and accumulator using BCD semantics.
     * Immediate result is allowed to overflow the normal byte range so that we can
     * use bit 8 as the overflow.
     */
    unsigned addBCDWithCarry(Byte iValue) noexcept {
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
    unsigned subBCDWithCarry(Byte iValue) noexcept {
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

    void addByteWithCarry(Byte iValue) noexcept {
        unsigned iSum = (iStatus & F_DECIMAL) ?
            addBCDWithCarry(iValue) :
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

    void subByteWithCarry(Byte iValue) noexcept {
        unsigned iDiff = (iStatus & F_DECIMAL) ?
            subBCDWithCarry(iValue) :
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

