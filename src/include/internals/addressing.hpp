#ifdef _INTERNALS_INCLUDES_

    /**
     * Support operations for yielding addressed operands
     */

    /**
    * $NN
    */
    Byte addrZeroPageByte() noexcept {
        return oOutside.readByte( // unsigned 8-bit value loaded from...
            iProgramCounter + 1    // operand byte
        );
    }

    /**
    * $NNNN
    */
    Word addrAbsoluteByte() noexcept {
        return readWord(         // unsigned 16-bit value loaded from...
            iProgramCounter + 1  // operand bytes
        );
    }

    /**
    * $NNNN,X
    */
    Word addrAbsoluteXByte() noexcept {
        return readWord(         // unsigned 16-bit value loaded from...
            iProgramCounter + 1  // operand bytes, offset by...
        ) + iXIndex;             // unsigned 8-bit index in X register
    }

    /**
    * $NNNN,Y
    */
    Word addrAbsoluteYByte() noexcept {
        return readWord(         // unsigned 16-bit value loaded from...
            iProgramCounter + 1  // operand bytes, offset by...
        ) + iYIndex;             // unsigned 8-bit index in Y register
    }

    /**
    * $NN,X (wraps in zero page)
    */
    Byte addrZeroPageXByte() noexcept {
        return (
            oOutside.readByte(       // unsigned 8-bit value loaded from...
                iProgramCounter + 1  // operand byte, offset by...
            ) + iXIndex              // unsigned 8-bit value in X register...
        ) /*& 0xFF*/;                // wrapped to zero page
    }

    /**
    * $NN,Y (wraps in zero page)
    */
    Byte addrZeroPageYByte() noexcept {
        return (
            oOutside.readByte(      // unsigned 8-bit value loaded from...
                iProgramCounter + 1 // operand byte, offset by...
            ) + iYIndex             // unsigned 8-bit value in Y register...
        ) /*& 0xFF*/;               // wrapped to zero page
    }

    /**
    * ($NN,X) (wraps in zero page)
    */
    Word addrPreIndexZeroPageXByte() noexcept {
        return readWord(                 // unsigned 16-bit value at address indicated by...
            (
                oOutside.readByte(       // unsigned 8-bit value loaded from...
                    iProgramCounter + 1  // operand byte, offset by...
                ) + iXIndex              // unsigned 8-bit value in X register...
            ) & 0xFF                     // wrapped to zero page
        );
    }

    /**
    * ($NN),Y
    */
    Word addrPostIndexZeroPageYByte() noexcept {
        return readWord(              // unsigned 16-bit value at address indicated by...
            oOutside.readByte(        // unsigned 8-bit value loaded from
                iProgramCounter + 1   // operand byte
            )                         // offset by...
        ) + iYIndex;                  //   unsigned 8-bit value in Y register
    }

#   define INTERNALS_ADDRESSING
#endif

