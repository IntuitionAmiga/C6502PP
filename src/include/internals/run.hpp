#ifdef _INTERNALS_INCLUDES_

// Step the program counter by the appropriate operation size defined in the enum
#define size(NAME) iProgramCounter += SIZE_ ## NAME

// Emit the opcode as an ASM comment
// When we move to a jump table, this macro will defuine the label
//#define handle(NAME) case NAME: asm("# handle(" #NAME ") -->");

#define OP(NAME) L_ ## NAME

#define handle(NAME) OP(NAME): asm("# handle(" #NAME ") -->");

// Jump Table Entry
#define JTE(NAME)  (uint16_t) ((uint8_t const*)&&OP(NAME) - (uint8_t const*)&&begin_interpreter)

// Conclude the operation.
// When we nove to a jump table, this macro will thread the computed goto.
//#define dispatch() break;
#define dispatch() ++iCount; goto *((uint8_t*)&&begin_interpreter + aJumpTable[oOutside.readByte(iProgramCounter)])

    size_t run() noexcept {

        alignas(NativeCacheLine) static uint16_t const aJumpTable[256] __attribute__((section(".text"))) = {
            JTE(BRK), // 0x00
            JTE(ORA_IX), // 0x01
            JTE(BAD), // 0x02 - illegal opcode
            JTE(BAD), // 0x03 - illegal opcode
            JTE(BAD), // 0x04 - illegal opcode
            JTE(ORA_ZP), // 0x05
            JTE(ASL_ZP), // 0x06
            JTE(BAD), // 0x07 - illegal opcode
            JTE(PHP), // 0x08
            JTE(ORA_IM), // 0x09
            JTE(ASL_A), // 0x0A
            JTE(BAD), // 0x0B - illegal opcode
            JTE(BAD), // 0x0C - illegal opcode
            JTE(ORA_AB), // 0x0D
            JTE(ASL_AB), // 0x0E
            JTE(BAD), // 0x0F - illegal opcode
            JTE(BPL), // 0x10
            JTE(ORA_IY), // 0x11
            JTE(BAD), // 0x12 - illegal opcode
            JTE(BAD), // 0x13 - illegal opcode
            JTE(BAD), // 0x14 - illegal opcode
            JTE(ORA_ZPX), // 0x15
            JTE(ASL_ZPX), // 0x16
            JTE(BAD), // 0x17 - illegal opcode
            JTE(CLC), // 0x18
            JTE(ORA_ABY), // 0x19
            JTE(BAD), // 0x1A - illegal opcode
            JTE(BAD), // 0x1B - illegal opcode
            JTE(BAD), // 0x1C - illegal opcode
            JTE(ORA_ABX), // 0x1D
            JTE(ASL_ABX), // 0x1E
            JTE(BAD), // 0x1F - illegal opcode
            JTE(JSR_AB), // 0x20
            JTE(AND_IX), // 0x21
            JTE(BAD), // 0x22 - illegal opcode
            JTE(BAD), // 0x23 - illegal opcode
            JTE(BIT_ZP), // 0x24
            JTE(AND_ZP), // 0x25
            JTE(ROL_ZP), // 0x26
            JTE(BAD), // 0x27 - illegal opcode
            JTE(PLP), // 0x28
            JTE(AND_IM), // 0x29
            JTE(ROL_A), // 0x2A
            JTE(BAD), // 0x2B - illegal opcode
            JTE(BIT_AB), // 0x2C
            JTE(AND_AB), // 0x2D
            JTE(ROL_AB), // 0x2E
            JTE(BAD), // 0x2F - illegal opcode
            JTE(BMI), // 0x30
            JTE(AND_IY), // 0x31
            JTE(BAD), // 0x32 - illegal opcode
            JTE(BAD), // 0x33 - illegal opcode
            JTE(BAD), // 0x34 - illegal opcode
            JTE(AND_ZPX), // 0x35
            JTE(ROL_ZPX), // 0x36
            JTE(BAD), // 0x37 - illegal opcode
            JTE(SEC), // 0x38
            JTE(AND_ABY), // 0x39
            JTE(BAD), // 0x3A - illegal opcode
            JTE(BAD), // 0x3B - illegal opcode
            JTE(BAD), // 0x3C - illegal opcode
            JTE(AND_ABX), // 0x3D
            JTE(ROL_ABX), // 0x3E
            JTE(BAD), // 0x3F - illegal opcode
            JTE(RTI), // 0x40
            JTE(EOR_IX), // 0x41
            JTE(BAD), // 0x42 - illegal opcode
            JTE(BAD), // 0x43 - illegal opcode
            JTE(BAD), // 0x44 - illegal opcode
            JTE(EOR_ZP), // 0x45
            JTE(LSR_ZP), // 0x46
            JTE(BAD), // 0x47 - illegal opcode
            JTE(PHA), // 0x48
            JTE(EOR_IM), // 0x49
            JTE(LSR_A), // 0x4A
            JTE(BAD), // 0x4B - illegal opcode
            JTE(JMP_AB), // 0x4C
            JTE(EOR_AB), // 0x4D
            JTE(LSR_AB), // 0x4E
            JTE(BAD), // 0x4F - illegal opcode
            JTE(BVC), // 0x50
            JTE(EOR_IY), // 0x51
            JTE(BAD), // 0x52 - illegal opcode
            JTE(BAD), // 0x53 - illegal opcode
            JTE(BAD), // 0x54 - illegal opcode
            JTE(EOR_ZPX), // 0x55
            JTE(LSR_ZPX), // 0x56
            JTE(BAD), // 0x57 - illegal opcode
            JTE(CLI), // 0x58
            JTE(EOR_ABY), // 0x59
            JTE(BAD), // 0x5A - illegal opcode
            JTE(BAD), // 0x5B - illegal opcode
            JTE(BAD), // 0x5C - illegal opcode
            JTE(EOR_ABX), // 0x5D
            JTE(LSR_ABX), // 0x5E
            JTE(BAD), // 0x5F - illegal opcode
            JTE(RTS), // 0x60
            JTE(ADC_IX), // 0x61
            JTE(BAD), // 0x62 - illegal opcode
            JTE(BAD), // 0x63 - illegal opcode
            JTE(BAD), // 0x64 - illegal opcode
            JTE(ADC_ZP), // 0x65
            JTE(ROR_ZP), // 0x66
            JTE(BAD), // 0x67 - illegal opcode
            JTE(PLA), // 0x68
            JTE(ADC_IM), // 0x69
            JTE(ROR_A), // 0x6A
            JTE(BAD), // 0x6B - illegal opcode
            JTE(JMP_IN), // 0x6C
            JTE(ADC_AB), // 0x6D
            JTE(ROR_AB), // 0x6E
            JTE(BAD), // 0x6F - illegal opcode
            JTE(BVS), // 0x70
            JTE(ADC_IY), // 0x71
            JTE(BAD), // 0x72 - illegal opcode
            JTE(BAD), // 0x73 - illegal opcode
            JTE(BAD), // 0x74 - illegal opcode
            JTE(ADC_ZPX), // 0x75
            JTE(ROR_ZPX), // 0x76
            JTE(BAD), // 0x77 - illegal opcode
            JTE(SEI), // 0x78
            JTE(ADC_ABY), // 0x79
            JTE(BAD), // 0x7A - illegal opcode
            JTE(BAD), // 0x7B - illegal opcode
            JTE(BAD), // 0x7C - illegal opcode
            JTE(ADC_ABX), // 0x7D
            JTE(ROR_ABX), // 0x7E
            JTE(BAD), // 0x7F - illegal opcode
            JTE(BAD), // 0x80 - illegal opcode
            JTE(STA_IX), // 0x81
            JTE(BAD), // 0x82 - illegal opcode
            JTE(BAD), // 0x83 - illegal opcode
            JTE(STY_ZP), // 0x84
            JTE(STA_ZP), // 0x85
            JTE(STX_ZP), // 0x86
            JTE(BAD), // 0x87 - illegal opcode
            JTE(DEY), // 0x88
            JTE(BAD), // 0x89 - illegal opcode
            JTE(TXA), // 0x8A
            JTE(BAD), // 0x8B - illegal opcode
            JTE(STY_AB), // 0x8C
            JTE(STA_AB), // 0x8D
            JTE(STX_AB), // 0x8E
            JTE(BAD), // 0x8F - illegal opcode
            JTE(BCC), // 0x90
            JTE(STA_IY), // 0x91
            JTE(BAD), // 0x92 - illegal opcode
            JTE(BAD), // 0x93 - illegal opcode
            JTE(STY_ZPX), // 0x94
            JTE(STA_ZPX), // 0x95
            JTE(STX_ZPY), // 0x96
            JTE(BAD), // 0x97 - illegal opcode
            JTE(TYA), // 0x98
            JTE(STA_ABY), // 0x99
            JTE(TXS), // 0x9A
            JTE(BAD), // 0x9B - illegal opcode
            JTE(BAD), // 0x9C - illegal opcode
            JTE(STA_ABX), // 0x9D
            JTE(BAD), // 0x9E - illegal opcode
            JTE(BAD), // 0x9F - illegal opcode
            JTE(LDY_IM), // 0xA0
            JTE(LDA_IX), // 0xA1
            JTE(LDX_IM), // 0xA2
            JTE(BAD), // 0xA3 - illegal opcode
            JTE(LDY_ZP), // 0xA4
            JTE(LDA_ZP), // 0xA5
            JTE(LDX_ZP), // 0xA6
            JTE(BAD), // 0xA7 - illegal opcode
            JTE(TAY), // 0xA8
            JTE(LDA_IM), // 0xA9
            JTE(TAX), // 0xAA
            JTE(BAD), // 0xAB - illegal opcode
            JTE(LDY_AB), // 0xAC
            JTE(LDA_AB), // 0xAD
            JTE(LDX_AB), // 0xAE
            JTE(BAD), // 0xAF - illegal opcode
            JTE(BCS), // 0xB0
            JTE(LDA_IY), // 0xB1
            JTE(BAD), // 0xB2 - illegal opcode
            JTE(BAD), // 0xB3 - illegal opcode
            JTE(LDY_ZPX), // 0xB4
            JTE(LDA_ZPX), // 0xB5
            JTE(LDX_ZPY), // 0xB6
            JTE(BAD), // 0xB7 - illegal opcode
            JTE(CLV), // 0xB8
            JTE(LDA_ABY), // 0xB9
            JTE(TSX), // 0xBA
            JTE(BAD), // 0xBB - illegal opcode
            JTE(LDY_ABX), // 0xBC
            JTE(LDA_ABX), // 0xBD
            JTE(LDX_ABY), // 0xBE
            JTE(BAD), // 0xBF - illegal opcode
            JTE(CPY_IM), // 0xC0
            JTE(CMP_IX), // 0xC1
            JTE(BAD), // 0xC2 - illegal opcode
            JTE(BAD), // 0xC3 - illegal opcode
            JTE(CPY_ZP), // 0xC4
            JTE(CMP_ZP), // 0xC5
            JTE(DEC_ZP), // 0xC6
            JTE(BAD), // 0xC7 - illegal opcode
            JTE(INY), // 0xC8
            JTE(CMP_IM), // 0xC9
            JTE(DEX), // 0xCA
            JTE(BAD), // 0xCB - illegal opcode
            JTE(CPY_AB), // 0xCC
            JTE(CMP_AB), // 0xCD
            JTE(DEC_AB), // 0xCE
            JTE(BAD), // 0xCF - illegal opcode
            JTE(BNE), // 0xD0
            JTE(CMP_IY), // 0xD1
            JTE(BAD), // 0xD2 - illegal opcode
            JTE(BAD), // 0xD3 - illegal opcode
            JTE(BAD), // 0xD4 - illegal opcode
            JTE(CMP_ZPX), // 0xD5
            JTE(DEC_ZPX), // 0xD6
            JTE(BAD), // 0xD7 - illegal opcode
            JTE(CLD), // 0xD8
            JTE(CMP_ABY), // 0xD9
            JTE(BAD), // 0xDA - illegal opcode
            JTE(BAD), // 0xDB - illegal opcode
            JTE(BAD), // 0xDC - illegal opcode
            JTE(CMP_ABX), // 0xDD
            JTE(DEC_ABX), // 0xDE
            JTE(BAD), // 0xDF - illegal opcode
            JTE(CPX_IM), // 0xE0
            JTE(SBC_IX), // 0xE1
            JTE(BAD), // 0xE2 - illegal opcode
            JTE(BAD), // 0xE3 - illegal opcode
            JTE(CPX_ZP), // 0xE4
            JTE(SBC_ZP), // 0xE5
            JTE(INC_ZP), // 0xE6
            JTE(BAD), // 0xE7 - illegal opcode
            JTE(INX), // 0xE8
            JTE(SBC_IM), // 0xE9
            JTE(NOP), // 0xEA
            JTE(BAD), // 0xEB - illegal opcode
            JTE(CPX_AB), // 0xEC
            JTE(SBC_AB), // 0xED
            JTE(INC_AB), // 0xEE
            JTE(BAD), // 0xEF - illegal opcode
            JTE(BEQ), // 0xF0
            JTE(SBC_IY), // 0xF1
            JTE(BAD), // 0xF2 - illegal opcode
            JTE(BAD), // 0xF3 - illegal opcode
            JTE(BAD), // 0xF4 - illegal opcode
            JTE(SBC_ZPX), // 0xF5
            JTE(INC_ZPX), // 0xF6
            JTE(BAD), // 0xF7 - illegal opcode
            JTE(SED), // 0xF8
            JTE(SBC_ABY), // 0xF9
            JTE(BAD), // 0xFA - illegal opcode
            JTE(BAD), // 0xFB - illegal opcode
            JTE(BAD), // 0xFC - illegal opcode
            JTE(SBC_ABX), // 0xFD
            JTE(INC_ABX), // 0xFE
            JTE(BAD), // 0xFF - illegal opcode

        };

        size_t iCount = 0;
        Word iAddress;
        Byte iValue, iCarry;

        begin_interpreter:
        dispatch();

        // Ordering by typical code frequency

        // Load Accumulator
        handle(LDA_IM)  updateNZ(
            iAccumulator = oOutside.readByte(iProgramCounter + 1)
        ); size(LDA_IM); dispatch();

        handle(LDA_ZP)  updateNZ(
            iAccumulator = oOutside.readByte(addrZeroPageByte())
        ); size(LDA_ZP);  dispatch();

        handle(LDA_ZPX) updateNZ(
            iAccumulator = oOutside.readByte(addrZeroPageXByte())
        ); size(LDA_ZPX);  dispatch();

        handle(LDA_AB)  updateNZ(
            iAccumulator = oOutside.readByte(addrAbsoluteByte())
        ); size(LDA_AB);  dispatch();

        handle(LDA_ABX) updateNZ(
            iAccumulator = oOutside.readByte(addrAbsoluteXByte())
        ); size(LDA_ABX);  dispatch();

        handle(LDA_ABY) updateNZ(
            iAccumulator = oOutside.readByte(addrAbsoluteYByte())
        ); size(LDA_ABY);  dispatch();

        handle(LDA_IX)  updateNZ(
            iAccumulator = oOutside.readByte(addrPreIndexZeroPageXByte())
        ); size(LDA_IX);  dispatch();

        handle(LDA_IY)  updateNZ(
            iAccumulator = oOutside.readByte(addrPostIndexZeroPageYByte())
        ); size(LDA_IY);  dispatch();

        // Store Accumulator
        handle(STA_ZP)  oOutside.writeByte(addrZeroPageByte(),  iAccumulator); size(STA_ZP); dispatch();
        handle(STA_ZPX) oOutside.writeByte(addrZeroPageXByte(), iAccumulator); size(STA_ZPX); dispatch();
        handle(STA_AB)  oOutside.writeByte(addrAbsoluteByte(),  iAccumulator); size(STA_AB); dispatch();
        handle(STA_ABX) oOutside.writeByte(addrAbsoluteXByte(), iAccumulator); size(STA_ABX); dispatch();
        handle(STA_ABY) oOutside.writeByte(addrAbsoluteYByte(), iAccumulator); size(STA_ABY); dispatch();
        handle(STA_IX)  oOutside.writeByte(addrPreIndexZeroPageXByte(),  iAccumulator); size(STA_IX); dispatch();
        handle(STA_IY)  oOutside.writeByte(addrPostIndexZeroPageYByte(), iAccumulator); size(STA_IY); dispatch();

        // Load X
        handle(LDX_IM)  updateNZ(iXIndex = oOutside.readByte(iProgramCounter + 1)); size(LDX_IM); dispatch();
        handle(LDX_ZP)  updateNZ(iXIndex = oOutside.readByte(addrZeroPageByte()));  size(LDX_ZP);  dispatch();
        handle(LDX_ZPY) updateNZ(iXIndex = oOutside.readByte(addrZeroPageYByte())); size(LDX_ZPY); dispatch();
        handle(LDX_AB)  updateNZ(iXIndex = oOutside.readByte(addrAbsoluteByte()));  size(LDX_AB); dispatch();
        handle(LDX_ABY) updateNZ(iXIndex = oOutside.readByte(addrAbsoluteYByte())); size(LDX_ABY); dispatch();

        // Store X
        handle(STX_ZP)  oOutside.writeByte(addrZeroPageByte(),  iXIndex); size(STX_ZP); dispatch();
        handle(STX_ZPY) oOutside.writeByte(addrZeroPageYByte(), iXIndex); size(STX_ZPY); dispatch();
        handle(STX_AB)  oOutside.writeByte(addrAbsoluteByte(),  iXIndex); size(STX_AB); dispatch();

        // Load Y
        handle(LDY_IM)  updateNZ(iYIndex = oOutside.readByte(iProgramCounter + 1)); size(LDY_IM); dispatch();
        handle(LDY_ZP)  updateNZ(iYIndex = oOutside.readByte(addrZeroPageByte()));  size(LDY_ZP); dispatch();
        handle(LDY_ZPX) updateNZ(iYIndex = oOutside.readByte(addrZeroPageXByte())); size(LDY_ZPX); dispatch();
        handle(LDY_AB)  updateNZ(iYIndex = oOutside.readByte(addrAbsoluteByte()));  size(LDY_AB); dispatch();
        handle(LDY_ABX) updateNZ(iYIndex = oOutside.readByte(addrAbsoluteXByte())); size(LDY_ABX); dispatch();

        // Store Y
        handle(STY_ZP)  oOutside.writeByte(addrZeroPageByte(),  iYIndex); size(STY_ZP); dispatch();
        handle(STY_ZPX) oOutside.writeByte(addrZeroPageXByte(), iYIndex); size(STY_ZPX); dispatch();
        handle(STY_AB)  oOutside.writeByte(addrAbsoluteByte(),  iYIndex); size(STY_AB); dispatch();

        // Compare
        // A - M
        handle(CMP_IM)  cmpByte(iAccumulator, oOutside.readByte(iProgramCounter + 1)); size(CMP_IM); dispatch();
        handle(CMP_ZP)  cmpByte(iAccumulator, oOutside.readByte(addrZeroPageByte()));  size(CMP_ZP); dispatch();
        handle(CMP_ZPX) cmpByte(iAccumulator, oOutside.readByte(addrZeroPageXByte())); size(CMP_ZPX); dispatch();
        handle(CMP_AB)  cmpByte(iAccumulator, oOutside.readByte(addrAbsoluteByte()));  size(CMP_AB); dispatch();
        handle(CMP_ABX) cmpByte(iAccumulator, oOutside.readByte(addrAbsoluteXByte())); size(CMP_ABX); dispatch();
        handle(CMP_ABY) cmpByte(iAccumulator, oOutside.readByte(addrAbsoluteYByte())); size(CMP_ABY); dispatch();
        handle(CMP_IX)  cmpByte(iAccumulator, oOutside.readByte(addrPreIndexZeroPageXByte()));  size(CMP_IX); dispatch();
        handle(CMP_IY)  cmpByte(iAccumulator, oOutside.readByte(addrPostIndexZeroPageYByte())); size(CMP_IY); dispatch();

        handle(CPX_IM)  cmpByte(iXIndex, oOutside.readByte(iProgramCounter + 1)); size(CPX_IM); dispatch();
        handle(CPX_ZP)  cmpByte(iXIndex, oOutside.readByte(addrZeroPageByte()));  size(CPX_ZP); dispatch();
        handle(CPX_AB)  cmpByte(iXIndex, oOutside.readByte(addrAbsoluteByte()));  size(CPX_AB); dispatch();

        handle(CPY_IM)  cmpByte(iYIndex, oOutside.readByte(iProgramCounter + 1)); size(CPY_IM); dispatch();
        handle(CPY_ZP)  cmpByte(iYIndex, oOutside.readByte(addrZeroPageByte()));  size(CPY_ZP); dispatch();
        handle(CPY_AB)  cmpByte(iYIndex, oOutside.readByte(addrAbsoluteByte()));  size(CPY_AB); dispatch();

        // Conditional
        handle(BCC) {
            iProgramCounter += (!(iStatus & F_CARRY)) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BCC
                : SIZE_BCC;
            dispatch();
        }

        handle(BCS) {
            iProgramCounter += (iStatus & F_CARRY) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BCS
                : SIZE_BCS;
            dispatch();
        }

        handle(BEQ) {
            iProgramCounter += (iStatus & F_ZERO) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BEQ
                : SIZE_BEQ;
            dispatch();
        }

        handle(BNE) {
            iProgramCounter += (!(iStatus & F_ZERO)) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BNE
                : SIZE_BNE;
            dispatch();
        }

        handle(BMI) {
            iProgramCounter += (iStatus & F_NEGATIVE) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BMI
                : SIZE_BMI;
            dispatch();
        }

        handle(BPL) {
            iProgramCounter += (!(iStatus & F_NEGATIVE)) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BPL
                : SIZE_BPL;
            dispatch();
        }

        handle(BVC) {
            iProgramCounter += (!(iStatus & F_OVERFLOW)) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BVC
                : SIZE_BVC;
            dispatch();
        }

        handle(BVS) {
            iProgramCounter += (iStatus & F_OVERFLOW) ?
                (int8_t)oOutside.readByte(iProgramCounter + 1) + SIZE_BVS
                : SIZE_BVS;
            dispatch();
        }


        handle(NOP) size(NOP); dispatch();

        // Status mangling
        handle(CLC) {
            iStatus &= ~F_CARRY;
            size(CLC);
            dispatch();
        }

        handle(CLD) iStatus &= ~F_DECIMAL;   size(CLD); dispatch();
        handle(CLI) iStatus &= ~F_INTERRUPT; size(CLI); dispatch();
        handle(CLV) iStatus &= ~F_OVERFLOW;  size(CLV); dispatch();
        handle(SEC) iStatus |= F_CARRY;      size(SEC); dispatch();
        handle(SED) iStatus |= F_DECIMAL;    size(SED); dispatch();
        handle(SEI) iStatus |= F_INTERRUPT;  size(SEI); dispatch();

        // Register transfer
        handle(TAX) updateNZ(iXIndex = iAccumulator);  size(TAX); dispatch();
        handle(TAY) updateNZ(iYIndex = iAccumulator);  size(TAY); dispatch();
        handle(TSX) updateNZ(iXIndex = iStackPointer); size(TSX); dispatch();
        handle(TXA) updateNZ(iAccumulator  = iXIndex); size(TXA); dispatch();
        // klausd tests: TXS does not update NZ
        handle(TXS) iStackPointer = iXIndex;           size(TXS); dispatch();
        handle(TYA) updateNZ(iAccumulator = iYIndex);  size(TYA); dispatch();

        // Stack
        handle(PHA) pushByte(iAccumulator); size(PHA); dispatch();
        handle(PHP) pushByte(iStatus | F_BREAK | F_UNUSED); size(PHP); dispatch();
        handle(PLA) updateNZ(iAccumulator = pullByte()); size(PLA); dispatch();
        handle(PLP) {
            iValue = pullByte() & ~(F_BREAK | F_UNUSED);
            iStatus = (iStatus & (F_BREAK | F_UNUSED)) | iValue;
            size(PLP);
            dispatch();
        }

        // Decrement
        handle(DEX) updateNZ(--iXIndex); size(DEX); dispatch();
        handle(DEY) updateNZ(--iYIndex); size(DEY); dispatch();

        handle(DEC_ZP) {
            iAddress = addrZeroPageByte();
            iValue   = (oOutside.readByte(iAddress) - 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(DEC_ZP);
            dispatch();
        }

        handle(DEC_ZPX) {
            iAddress = addrZeroPageXByte();
            iValue   = (oOutside.readByte(iAddress) - 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(DEC_ZPX);
            dispatch();
        }

        handle(DEC_AB) {
            iAddress = addrAbsoluteByte();
            iValue   = (oOutside.readByte(iAddress) - 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(DEC_AB);
            dispatch();
        }

        handle(DEC_ABX) {
            iAddress = addrAbsoluteXByte();
            iValue   = (oOutside.readByte(iAddress) - 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(DEC_ABX);
            dispatch();
        }


        // Increment
        handle(INX) updateNZ(++iXIndex); size(INX); dispatch();
        handle(INY) updateNZ(++iYIndex); size(INY); dispatch();

        handle(INC_ZP) {
            iAddress = addrZeroPageByte();
            iValue   = (oOutside.readByte(iAddress) + 1);
            updateNZ(iValue );
            oOutside.writeByte(iAddress, iValue);
            size(INC_ZP);
            dispatch();
        }

        handle(INC_ZPX) {
            iAddress = addrZeroPageXByte();
            iValue   = (oOutside.readByte(iAddress) + 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(INC_ZPX);
            dispatch();
        }

        handle(INC_AB) {
            iAddress = addrAbsoluteByte();
            iValue   = (oOutside.readByte(iAddress) + 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(INC_AB);
            dispatch();
        }

        handle(INC_ABX) {
            iAddress = addrAbsoluteXByte();
            iValue   = (oOutside.readByte(iAddress) + 1);
            updateNZ(iValue);
            oOutside.writeByte(iAddress, iValue);
            size(INC_ABX);
            dispatch();
        }



        // Logic Ops...
        handle(AND_IM)  updateNZ(iAccumulator &= oOutside.readByte(iProgramCounter + 1)); size(AND_IM); dispatch();
        handle(AND_ZP)  updateNZ(iAccumulator &= oOutside.readByte(addrZeroPageByte()));  size(AND_ZP); dispatch();
        handle(AND_ZPX) updateNZ(iAccumulator &= oOutside.readByte(addrZeroPageXByte())); size(AND_ZPX); dispatch();
        handle(AND_AB)  updateNZ(iAccumulator &= oOutside.readByte(addrAbsoluteByte()));  size(AND_AB); dispatch();
        handle(AND_ABX) updateNZ(iAccumulator &= oOutside.readByte(addrAbsoluteXByte())); size(AND_ABX); dispatch();
        handle(AND_ABY) updateNZ(iAccumulator &= oOutside.readByte(addrAbsoluteYByte())); size(AND_ABY); dispatch();
        handle(AND_IX)  updateNZ(iAccumulator &= oOutside.readByte(addrPreIndexZeroPageXByte()));  size(AND_IX); dispatch();
        handle(AND_IY)  updateNZ(iAccumulator &= oOutside.readByte(addrPostIndexZeroPageYByte())); size(AND_IY); dispatch();
        handle(ORA_IM)  updateNZ(iAccumulator |= oOutside.readByte(iProgramCounter + 1)); size(ORA_IM); dispatch();
        handle(ORA_ZP)  updateNZ(iAccumulator |= oOutside.readByte(addrZeroPageByte())); size(ORA_ZP);  dispatch();
        handle(ORA_ZPX) updateNZ(iAccumulator |= oOutside.readByte(addrZeroPageXByte())); size(ORA_ZPX); dispatch();
        handle(ORA_AB)  updateNZ(iAccumulator |= oOutside.readByte(addrAbsoluteByte()));  size(ORA_AB); dispatch();
        handle(ORA_ABX) updateNZ(iAccumulator |= oOutside.readByte(addrAbsoluteXByte())); size(ORA_ABX); dispatch();
        handle(ORA_ABY) updateNZ(iAccumulator |= oOutside.readByte(addrAbsoluteYByte())); size(ORA_ABY); dispatch();
        handle(ORA_IX)  updateNZ(iAccumulator |= oOutside.readByte(addrPreIndexZeroPageXByte()));  size(ORA_IX); dispatch();
        handle(ORA_IY)  updateNZ(iAccumulator |= oOutside.readByte(addrPostIndexZeroPageYByte())); size(ORA_IY); dispatch();
        handle(EOR_IM)  updateNZ(iAccumulator ^= oOutside.readByte(iProgramCounter + 1)); size(EOR_IM); dispatch();
        handle(EOR_ZP)  updateNZ(iAccumulator ^= oOutside.readByte(addrZeroPageByte()));  size(EOR_ZP); dispatch();
        handle(EOR_ZPX) updateNZ(iAccumulator ^= oOutside.readByte(addrZeroPageXByte())); size(EOR_ZPX); dispatch();
        handle(EOR_AB)  updateNZ(iAccumulator ^= oOutside.readByte(addrAbsoluteByte()));  size(EOR_AB); dispatch();
        handle(EOR_ABX) updateNZ(iAccumulator ^= oOutside.readByte(addrAbsoluteXByte())); size(EOR_ABX); dispatch();
        handle(EOR_ABY) updateNZ(iAccumulator ^= oOutside.readByte(addrAbsoluteYByte())); size(EOR_ABY); dispatch();
        handle(EOR_IX)  updateNZ(iAccumulator ^= oOutside.readByte(addrPreIndexZeroPageXByte()));  size(EOR_IX); dispatch();
        handle(EOR_IY)  updateNZ(iAccumulator ^= oOutside.readByte(addrPostIndexZeroPageYByte())); size(EOR_IY); dispatch();

        // Arithmetuc shift left
        handle(ASL_A) {
            iStatus &= ~F_CARRY;
            iStatus |= (iAccumulator & F_NEGATIVE) >> 7; // sign -> carry
            updateNZ(iAccumulator <<= 1);
            size(ASL_A);
            dispatch();
        }

        handle(ASL_ZP)  aslMemory(addrZeroPageByte()); size(ASL_ZP); dispatch();
        handle(ASL_ZPX) aslMemory(addrZeroPageXByte()); size(ASL_ZPX); dispatch();
        handle(ASL_AB)  aslMemory(addrAbsoluteByte()); size(ASL_AB); dispatch();
        handle(ASL_ABX) aslMemory(addrAbsoluteXByte()); size(ASL_ABX); dispatch();

        // Logical shift right
        handle(LSR_A) {
            iStatus &= ~F_CARRY;
            iStatus |= (iAccumulator & F_CARRY);
            updateNZ(iAccumulator >>= 1);
            size(LSR_A);
            dispatch();
        }

        handle(ROL_A) {
            iCarry = (iStatus & F_CARRY);
            iStatus &= ~F_CARRY;
            iStatus |= (iAccumulator & F_NEGATIVE) >> 7; // sign -> carry
            updateNZ( iAccumulator = ((iAccumulator << 1) | iCarry) );
            size(ROL_A);
            dispatch();
        }

        handle(ROL_ZP)  rolMemory(addrZeroPageByte()); size(ROL_ZP); dispatch();
        handle(ROL_ZPX) rolMemory(addrZeroPageXByte()); size(ROL_ZPX); dispatch();
        handle(ROL_AB)  rolMemory(addrAbsoluteByte()); size(ROL_AB); dispatch();
        handle(ROL_ABX) rolMemory(addrAbsoluteXByte()); size(ROL_ABX); dispatch();

        handle(ROR_A) {
            iCarry = (iStatus & F_CARRY) << 7; // carry -> sign
            iStatus &= ~F_CARRY;
            iStatus |= (iAccumulator & F_CARRY); // carry -> carry
            updateNZ(iAccumulator = ((iAccumulator >> 1) | iCarry));
            size(ROR_A);
            dispatch();
        }

        handle(ROR_ZP)  rorMemory(addrZeroPageByte());  size(ROR_ZP); dispatch();
        handle(ROR_ZPX) rorMemory(addrZeroPageXByte()); size(ROR_ZPX); dispatch();
        handle(ROR_AB)  rorMemory(addrAbsoluteByte());  size(ROR_AB); dispatch();
        handle(ROR_ABX) rorMemory(addrAbsoluteXByte()); size(ROR_ABX); dispatch();

        handle(LSR_ZP)  lsrMemory(addrZeroPageByte());  size(LSR_ZP); dispatch();
        handle(LSR_ZPX) lsrMemory(addrZeroPageXByte()); size(LSR_ZPX); dispatch();
        handle(LSR_AB)  lsrMemory(addrAbsoluteByte()); size(LSR_AB); dispatch();
        handle(LSR_ABX) lsrMemory(addrAbsoluteXByte()); size(LSR_ABX); dispatch();


        // Addition
        // A + M + C
        handle(ADC_IM)  addByteWithCarry(oOutside.readByte(iProgramCounter + 1)); size(ADC_IM); dispatch();
        handle(ADC_ZP)  addByteWithCarry(oOutside.readByte(addrZeroPageByte()));  size(ADC_ZP); dispatch();
        handle(ADC_ZPX) addByteWithCarry(oOutside.readByte(addrZeroPageXByte())); size(ADC_ZPX); dispatch();
        handle(ADC_AB)  addByteWithCarry(oOutside.readByte(addrAbsoluteByte()));  size(ADC_AB); dispatch();
        handle(ADC_ABX) addByteWithCarry(oOutside.readByte(addrAbsoluteXByte())); size(ADC_ABX); dispatch();
        handle(ADC_ABY) addByteWithCarry(oOutside.readByte(addrAbsoluteYByte())); size(ADC_ABY); dispatch();
        handle(ADC_IX)  addByteWithCarry(oOutside.readByte(addrPreIndexZeroPageXByte()));  size(ADC_IX); dispatch();
        handle(ADC_IY)  addByteWithCarry(oOutside.readByte(addrPostIndexZeroPageYByte())); size(ADC_IY); dispatch();

        // Subtract
        // A - M - B => A + (255 - M) - (1 - C) => A + ~M + C
        handle(SBC_IM)  subByteWithCarry(oOutside.readByte(iProgramCounter + 1)); size(SBC_IM); dispatch();
        handle(SBC_ZP)  subByteWithCarry(oOutside.readByte(addrZeroPageByte()));  size(SBC_ZP); dispatch();
        handle(SBC_ZPX) subByteWithCarry(oOutside.readByte(addrZeroPageXByte())); size(SBC_ZPX); dispatch();
        handle(SBC_AB)  subByteWithCarry(oOutside.readByte(addrAbsoluteByte()));  size(SBC_AB); dispatch();
        handle(SBC_ABX) subByteWithCarry(oOutside.readByte(addrAbsoluteXByte())); size(SBC_ABX); dispatch();
        handle(SBC_ABY) subByteWithCarry(oOutside.readByte(addrAbsoluteYByte())); size(SBC_ABY); dispatch();
        handle(SBC_IX)  subByteWithCarry(oOutside.readByte(addrPreIndexZeroPageXByte()));  size(SBC_IX); dispatch();
        handle(SBC_IY)  subByteWithCarry(oOutside.readByte(addrPostIndexZeroPageYByte())); size(SBC_IY); dispatch();



        handle(BIT_ZP) {
            iValue = oOutside.readByte(addrZeroPageByte());
            iStatus &= F_CLR_NZV;
            iStatus |= (iValue & (F_NEGATIVE | F_OVERFLOW)) | (
                iValue & iAccumulator ? 0 : F_ZERO
            );
            size(BIT_ZP);
            dispatch();
        }
        handle(BIT_AB) {
            iValue = oOutside.readByte(addrAbsoluteByte());
            iStatus &= F_CLR_NZV;
            iStatus |= (iValue & (F_NEGATIVE|F_OVERFLOW)) | (
                iValue & iAccumulator ? 0 : F_ZERO
            );
            size(BIT_AB);
            dispatch();
        }


        // unconditional
        handle(JMP_AB) {
            //iCycles += OP_CYCLES[iOpcode];

            iAddress = readWord(iProgramCounter + 1);

            if (iAddress == iProgramCounter) {
                // Hard Infinite Loop
                return iCount;
            }

            iProgramCounter = iAddress;

            dispatch();
            // Avoid the program counter update, since we releaded it anyway
            //return true;
        }

        handle(JMP_IN) {
            // Emulate the 6502 indirect jump bug with respect to page boundaries.
            Address iPointerAddress = readWord(iProgramCounter + 1);
            if (0xFF == (iPointerAddress & 0xFF)) {
                iAddress = oOutside.readByte(iPointerAddress);
                iAddress |= oOutside.readByte(iPointerAddress & 0xFF00) << 8;
                //iProgramCounter = readWord(iAddress);
                iProgramCounter = iAddress;
            } else {
                iProgramCounter = readWord(iPointerAddress);
            }
            dispatch();
            //return true;
        }

        handle(JSR_AB) {
            // Note the 6502 notion of the return address is actually the address of the last byte of
            // the operation.
            iAddress = (iProgramCounter + 2);
            pushByte(iAddress >> 8);
            pushByte(iAddress & 0xFF);
            iProgramCounter = readWord(iProgramCounter + 1);
            dispatch();
            //return true;
        }

        handle(RTS) {
            iAddress  = pullByte();
            iAddress |= (pullByte() << 8);
            iProgramCounter = iAddress + 1;
            dispatch();
            //return true;
        }

        handle(RTI) {
            // Pull SR but ignore bit 5
            iValue = pullByte() & ~(F_UNUSED|F_BREAK); // clear unused only
            iStatus &= (F_UNUSED|F_BREAK); // clear all but unused flag
            iStatus |= iValue;

            // Pull PC
            iAddress  = pullByte();
            iAddress |= (pullByte() << 8);
            iProgramCounter = iAddress;// + 1;
            dispatch();
            //return true;
        }

        handle(BRK) {
            // Push PC+2 as return address
            //iValAddress    = iProgramCounter + 1;
            iAddress = (iProgramCounter + 2);
            pushByte(iAddress >> 8);
            pushByte(iAddress & 0xFF);

            // Push SR
            pushByte(iStatus|F_BREAK|F_UNUSED);

            // Reload PC from IRQ vector
            iProgramCounter = readWord(VEC_IRQ);

            // Set interrupted status. Is this the correct location?
            iStatus |= F_INTERRUPT;
            dispatch();
            //return true;
        }

        handle(BAD)
        return iCount;

    }

#   define INTERNALS_STEP
#endif

