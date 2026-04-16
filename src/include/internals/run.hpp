#ifdef _INTERNALS_INCLUDES_

#ifdef UNPINNED
#   define pin()
#   define unpin()
#else
#define pin() \
    auto& __restrict__ oOutside = this->oOutside; \
    Address iProgramCounter = this->iProgramCounter; \
    Byte iStatus = this->iStatus; \
    Byte iAccumulator = this->iAccumulator;
#define unpin() \
    this->iProgramCounter = iProgramCounter; \
    this->iStatus = iStatus; \
    this->iAccumulator = iAccumulator;
#endif


#ifdef INTERPRET_SWITCH // switch-case
#   define size(NAME) iProgramCounter += SIZE_ ## NAME
#   define OP(NAME)
#   define handle(NAME) case NAME: asm("# case " #NAME ": -->");
#   define dispatch() break;
#   define begin() for (;;) switch (oOutside.readByte(iProgramCounter))
#   define done()
#   define illegal() default: unpin(); return *this;

#else // Jump Table interpeter

#define size(NAME) iProgramCounter += SIZE_ ## NAME

#   ifdef WIDE_JUMP
using Jump = uint32_t;
#   else
using Jump = uint16_t;
#   endif

#   define OP(NAME) L_ ## NAME
#   define handle(NAME) OP(NAME): asm("# handle(" #NAME ") -->");
#   define JTE(NAME)  (Jump) ((uint8_t const*)&&OP(NAME) - (uint8_t const*)&&begin_interpreter)
#   define dispatch() goto *((uint8_t*)&&begin_interpreter + aJumpTable[oOpcodeObserver.observe(oOutside.readByte(iProgramCounter))])
#   define begin() \
    begin_interpreter: \
    dispatch();
#   define illegal() handle(BAD)

#endif

#define load(addr) oOutside.readByte(addr)
#define store(addr,val) oOutside.writeByte(addr, val)

#define pull() (oOutside.readByte(++iStackPointer + STACK_BASE))
#define push(byte) oOutside.writeByte(STACK_BASE + iStackPointer--, byte)

#define lsrm(addr) oOutside.writeByte( \
            iAddress, \
            shiftRightWithCarry(oOutside.readByte((iAddress = addr))) \
        )

// Addressing mode macros. These call the static inline functions passing the const qualified paramters
// to assist with inlining using any pinned values.
#define zp()  addrZeroPageByte(oOutside, iProgramCounter)
#define ab()  addrAbsoluteByte(oOutside, iProgramCounter)
#define abx() addrAbsoluteXByte(oOutside, iProgramCounter, iXIndex)
#define aby() addrAbsoluteYByte(oOutside, iProgramCounter, iYIndex)
#define zpx() addrZeroPageXByte(oOutside, iProgramCounter, iXIndex)
#define zpy() addrZeroPageYByte(oOutside, iProgramCounter, iYIndex)
#define ix()  addrPreIndexZeroPageXByte(oOutside, iProgramCounter, iXIndex)
#define iy()  addrPostIndexZeroPageYByte(oOutside, iProgramCounter, iYIndex)

    /**
     * Main run entry point. We need to make this interruptable, really.
     */
    MOS6502& run() noexcept __attribute__((hot)) {

#ifndef INTERPRET_SWITCH
        alignas(NativeCacheLine) static Jump const aJumpTable[256] __attribute__((section(".text"))) = {
            JTE(BRK), // 0x00
            JTE(ORA_IX), // 0x01
            JTE(BAD),//JTE(JAM), // 0x02 - JAM (Halt)
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

#endif

        pin();

        Word iAddress;
        Byte iValue, iCarry;

        begin() {

            // Ordering by typical code frequency

            // Load Accumulator
            handle(LDA_IM) {
                updateNZ(iStatus, iAccumulator = load(iProgramCounter + 1));
                size(LDA_IM);
                dispatch();
            }

            handle(LDA_ZP) {
                updateNZ(iStatus, iAccumulator = load(zp()));
                size(LDA_ZP);
                dispatch();
            }

            handle(LDA_ZPX) {
                updateNZ(iStatus, iAccumulator = load(zpx()));
                size(LDA_ZPX);
                dispatch();
            }

            handle(LDA_AB) {
                updateNZ(iStatus, iAccumulator = load(ab()));
                size(LDA_AB);
                dispatch();
            }

            handle(LDA_ABX) {
                updateNZ(iStatus, iAccumulator = load(abx()));
                size(LDA_ABX);
                dispatch();
            }

            handle(LDA_ABY) {
                updateNZ(iStatus, iAccumulator = load(aby()));
                size(LDA_ABY);
                dispatch();
            }

            handle(LDA_IX) {
                updateNZ(iStatus, iAccumulator = load(ix()));
                size(LDA_IX);
                dispatch();
            }

            handle(LDA_IY) {
                updateNZ(iStatus, iAccumulator = load(iy()));
                size(LDA_IY);
                dispatch();
            }

            // Store Accumulator
            handle(STA_ZP) {
                store(zp(),  iAccumulator);
                size(STA_ZP);
                dispatch();
            }

            handle(STA_ZPX) {
                store(zpx(), iAccumulator);
                size(STA_ZPX);
                dispatch();
            }

            handle(STA_AB) {
                store(ab(),  iAccumulator);
                size(STA_AB);
                dispatch();
            }

            handle(STA_ABX) {
                store(abx(), iAccumulator);
                size(STA_ABX);
                dispatch();
            }

            handle(STA_ABY) {
                store(aby(), iAccumulator);
                size(STA_ABY);
                dispatch();
            }

            handle(STA_IX) {
                store(ix(),  iAccumulator);
                size(STA_IX);
                dispatch();
            }

            handle(STA_IY) {
                store(iy(), iAccumulator);
                size(STA_IY);
                dispatch();
            }

            // Load X
            handle(LDX_IM) {
                updateNZ(iStatus, iXIndex = load(iProgramCounter + 1));
                size(LDX_IM);
                dispatch();
            }

            handle(LDX_ZP) {
                updateNZ(iStatus, iXIndex = load(zp()));
                size(LDX_ZP);
                dispatch();
            }

            handle(LDX_ZPY) {
                updateNZ(iStatus, iXIndex = load(zpy()));
                size(LDX_ZPY);
                dispatch();
            }

            handle(LDX_AB) {
                updateNZ(iStatus, iXIndex = load(ab()));
                size(LDX_AB);
                dispatch();
            }

            handle(LDX_ABY) {
                updateNZ(iStatus, iXIndex = load(aby()));
                size(LDX_ABY);
                dispatch();
            }

            // Store X
            handle(STX_ZP) {
                store(zp(),  iXIndex);
                size(STX_ZP);
                dispatch();
            }

            handle(STX_ZPY) {
                store(zpy(), iXIndex);
                size(STX_ZPY);
                dispatch();
            }

            handle(STX_AB) {
                store(ab(),  iXIndex);
                size(STX_AB);
                dispatch();
            }

            // Load Y
            handle(LDY_IM) {
                updateNZ(iStatus, iYIndex = load(iProgramCounter + 1));
                size(LDY_IM);
                dispatch();
            }

            handle(LDY_ZP) {
                updateNZ(iStatus, iYIndex = load(zp()));
                size(LDY_ZP);
                dispatch();
            }

            handle(LDY_ZPX) {
                updateNZ(iStatus, iYIndex = load(zpx()));
                size(LDY_ZPX);
                dispatch();
            }

            handle(LDY_AB) {
                updateNZ(iStatus, iYIndex = load(ab()));
                size(LDY_AB);
                dispatch();
            }

            handle(LDY_ABX) {
                updateNZ(iStatus, iYIndex = load(abx()));
                size(LDY_ABX);
                dispatch();
            }

            // Store Y
            handle(STY_ZP) {
                store(zp(),  iYIndex);
                size(STY_ZP);
                dispatch();
            }

            handle(STY_ZPX) {
                store(zpx(), iYIndex);
                size(STY_ZPX);
                dispatch();
            }

            handle(STY_AB) {
                store(ab(),  iYIndex);
                size(STY_AB);
                dispatch();
            }

            // Compare
            // A - M {
            handle(CMP_IM) {
                cmpByte(iStatus, iAccumulator, load(iProgramCounter + 1));
                size(CMP_IM);
                dispatch();
            }

            handle(CMP_ZP) {
                cmpByte(iStatus, iAccumulator, load(zp()));
                size(CMP_ZP);
                dispatch();
            }

            handle(CMP_ZPX) {
                cmpByte(iStatus, iAccumulator, load(zpx()));
                size(CMP_ZPX);
                dispatch();
            }

            handle(CMP_AB) {
                cmpByte(iStatus, iAccumulator, load(ab()));
                size(CMP_AB);
                dispatch();
            }

            handle(CMP_ABX) {
                cmpByte(iStatus, iAccumulator, load(abx()));
                size(CMP_ABX);
                dispatch();
            }

            handle(CMP_ABY) {
                cmpByte(iStatus, iAccumulator, load(aby()));
                size(CMP_ABY);
                dispatch();
            }

            handle(CMP_IX) {
                cmpByte(iStatus, iAccumulator, load(ix()));
                size(CMP_IX);
                dispatch();
            }

            handle(CMP_IY) {
                cmpByte(iStatus, iAccumulator, load(iy()));
                size(CMP_IY);
                dispatch();
            }

            handle(CPX_IM) {
                cmpByte(iStatus, iXIndex, load(iProgramCounter + 1));
                size(CPX_IM);
                dispatch();
            }

            handle(CPX_ZP) {
                cmpByte(iStatus, iXIndex, load(zp()));
                size(CPX_ZP);
                dispatch();
            }

            handle(CPX_AB) {
                cmpByte(iStatus, iXIndex, load(ab()));
                size(CPX_AB);
                dispatch();
            }

            handle(CPY_IM) {
                cmpByte(iStatus, iYIndex, load(iProgramCounter + 1));
                size(CPY_IM);
                dispatch();
            }

            handle(CPY_ZP) {
                cmpByte(iStatus, iYIndex, load(zp()));
                size(CPY_ZP);
                dispatch();
            }

            handle(CPY_AB) {
                cmpByte(iStatus, iYIndex, load(ab()));
                size(CPY_AB);
                dispatch();
            }

            // Conditional
            handle(BCC) {
                iProgramCounter += (!(iStatus & F_CARRY)) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BCC
                    : SIZE_BCC;
                dispatch();
            }

            handle(BCS) {
                iProgramCounter += (iStatus & F_CARRY) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BCS
                    : SIZE_BCS;
                dispatch();
            }

            handle(BEQ) {
                iProgramCounter += (iStatus & F_ZERO) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BEQ
                    : SIZE_BEQ;
                dispatch();
            }

            handle(BNE) {
                iProgramCounter += (!(iStatus & F_ZERO)) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BNE
                    : SIZE_BNE;
                dispatch();
            }

            handle(BMI) {
                iProgramCounter += (iStatus & F_NEGATIVE) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BMI
                    : SIZE_BMI;
                dispatch();
            }

            handle(BPL) {
                iProgramCounter += (!(iStatus & F_NEGATIVE)) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BPL
                    : SIZE_BPL;
                dispatch();
            }

            handle(BVC) {
                iProgramCounter += (!(iStatus & F_OVERFLOW)) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BVC
                    : SIZE_BVC;
                dispatch();
            }

            handle(BVS) {
                iProgramCounter += (iStatus & F_OVERFLOW) ?
                    (int8_t)load(iProgramCounter + 1) + SIZE_BVS
                    : SIZE_BVS;
                dispatch();
            }


            handle(NOP) {
                size(NOP);
                dispatch();
            }

            // Status mangling
            handle(CLC) {
                iStatus &= ~F_CARRY;
                size(CLC);
                dispatch();
            }

            handle(CLD) {
                iStatus &= ~F_DECIMAL;
                size(CLD);
                dispatch();
            }

            handle(CLI) {
                iStatus &= ~F_INTERRUPT;
                size(CLI);
                dispatch();
            }

            handle(CLV) {
                iStatus &= ~F_OVERFLOW;
                size(CLV);
                dispatch();
            }

            handle(SEC) {
                iStatus |= F_CARRY;
                size(SEC);
                dispatch();
            }

            handle(SED) {
                iStatus |= F_DECIMAL;
                size(SED);
                dispatch();
            }

            handle(SEI) {
                iStatus |= F_INTERRUPT;
                size(SEI);
                dispatch();
            }

            // Register transfer
            handle(TAX) {
                updateNZ(iStatus, iXIndex = iAccumulator);
                size(TAX);
                dispatch();
            }

            handle(TAY) {
                updateNZ(iStatus, iYIndex = iAccumulator);
                size(TAY);
                dispatch();
            }

            handle(TSX) {
                updateNZ(iStatus, iXIndex = iStackPointer);
                size(TSX);
                dispatch();
            }

            handle(TXA) {
                updateNZ(iStatus, iAccumulator  = iXIndex);
                size(TXA);
                dispatch();
            }

            // klausd tests: TXS does not update NZ
            handle(TXS) {
                iStackPointer = iXIndex;
                size(TXS);
                dispatch();
            }

            handle(TYA) {
                updateNZ(iStatus, iAccumulator = iYIndex);
                size(TYA);
                dispatch();
            }

            // Stack
            handle(PHA) {
                push(iAccumulator);
                size(PHA);
                dispatch();
            }

            handle(PHP) {
                // PHP... SixPhpive02 Rides Again
                push(iStatus | F_BREAK | F_UNUSED);
                size(PHP);
                dispatch();
            }

            handle(PLA) {
                updateNZ(iStatus, iAccumulator = pull());
                size(PLA);
                dispatch();
            }

            handle(PLP) {
                iValue = pull() & ~(F_BREAK | F_UNUSED);
                iStatus = (iStatus & (F_BREAK | F_UNUSED)) | iValue;
                size(PLP);
                dispatch();
            }

            // Decrement
            handle(DEX) {
                updateNZ(iStatus, --iXIndex);
                size(DEX);
                dispatch();
            }

            handle(DEY) {
                updateNZ(iStatus, --iYIndex);
                size(DEY);
                dispatch();
            }

            handle(DEC_ZP) {
                iAddress = zp();
                iValue   = (load(iAddress) - 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(DEC_ZP);
                dispatch();
            }

            handle(DEC_ZPX) {
                iAddress = zpx();
                iValue   = (load(iAddress) - 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(DEC_ZPX);
                dispatch();
            }

            handle(DEC_AB) {
                iAddress = ab();
                iValue   = (load(iAddress) - 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(DEC_AB);
                dispatch();
            }

            handle(DEC_ABX) {
                iAddress = abx();
                iValue   = (load(iAddress) - 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(DEC_ABX);
                dispatch();
            }


            // Increment
            handle(INX) {
                updateNZ(iStatus, ++iXIndex);
                size(INX);
                dispatch();
            }

            handle(INY) {
                updateNZ(iStatus, ++iYIndex);
                size(INY);
                dispatch();
            }

            handle(INC_ZP) {
                iAddress = zp();
                iValue   = (load(iAddress) + 1);
                updateNZ(iStatus, iValue );
                store(iAddress, iValue);
                size(INC_ZP);
                dispatch();
            }

            handle(INC_ZPX) {
                iAddress = zpx();
                iValue   = (load(iAddress) + 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(INC_ZPX);
                dispatch();
            }

            handle(INC_AB) {
                iAddress = ab();
                iValue   = (load(iAddress) + 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(INC_AB);
                dispatch();
            }

            handle(INC_ABX) {
                iAddress = abx();
                iValue   = (load(iAddress) + 1);
                updateNZ(iStatus, iValue);
                store(iAddress, iValue);
                size(INC_ABX);
                dispatch();
            }

            // Logic Ops...
            handle(AND_IM) {
                updateNZ(iStatus, iAccumulator &= load(iProgramCounter + 1));
                size(AND_IM);
                dispatch();
            }

            handle(AND_ZP) {
                updateNZ(iStatus, iAccumulator &= load(zp()));
                size(AND_ZP);
                dispatch();
            }

            handle(AND_ZPX) {
                updateNZ(iStatus, iAccumulator &= load(zpx()));
                size(AND_ZPX);
                dispatch();
            }

            handle(AND_AB) {
                updateNZ(iStatus, iAccumulator &= load(ab()));
                size(AND_AB);
                dispatch();
            }

            handle(AND_ABX) {
                updateNZ(iStatus, iAccumulator &= load(abx()));
                size(AND_ABX);
                dispatch();
            }

            handle(AND_ABY) {
                updateNZ(iStatus, iAccumulator &= load(aby()));
                size(AND_ABY);
                dispatch();
            }

            handle(AND_IX) {
                updateNZ(iStatus, iAccumulator &= load(ix()));
                size(AND_IX);
                dispatch();
            }

            handle(AND_IY) {
                updateNZ(iStatus, iAccumulator &= load(iy()));
                size(AND_IY);
                dispatch();
            }

            handle(ORA_IM) {
                updateNZ(iStatus, iAccumulator |= load(iProgramCounter + 1));
                size(ORA_IM);
                dispatch();
            }

            handle(ORA_ZP) {
                updateNZ(iStatus, iAccumulator |= load(zp()));
                size(ORA_ZP);
                dispatch();
            }

            handle(ORA_ZPX) {
                updateNZ(iStatus, iAccumulator |= load(zpx()));
                size(ORA_ZPX);
                dispatch();
            }

            handle(ORA_AB) {
                updateNZ(iStatus, iAccumulator |= load(ab()));
                size(ORA_AB);
                dispatch();
            }

            handle(ORA_ABX) {
                updateNZ(iStatus, iAccumulator |= load(abx()));
                size(ORA_ABX);
                dispatch();
            }

            handle(ORA_ABY) {
                updateNZ(iStatus, iAccumulator |= load(aby()));
                size(ORA_ABY);
                dispatch();
            }

            handle(ORA_IX) {
                updateNZ(iStatus, iAccumulator |= load(ix()));
                size(ORA_IX);
                dispatch();
            }

            handle(ORA_IY) {
                updateNZ(iStatus, iAccumulator |= load(iy()));
                size(ORA_IY);
                dispatch();
            }

            handle(EOR_IM) {
                updateNZ(iStatus, iAccumulator ^= load(iProgramCounter + 1));
                size(EOR_IM);
                dispatch();
            }

            handle(EOR_ZP) {
                updateNZ(iStatus, iAccumulator ^= load(zp()));
                size(EOR_ZP);
                dispatch();
            }

            handle(EOR_ZPX) {
                updateNZ(iStatus, iAccumulator ^= load(zpx()));
                size(EOR_ZPX);
                dispatch();
            }

            handle(EOR_AB) {
                updateNZ(iStatus, iAccumulator ^= load(ab()));
                size(EOR_AB);
                dispatch();
            }

            handle(EOR_ABX) {
                updateNZ(iStatus, iAccumulator ^= load(abx()));
                size(EOR_ABX);
                dispatch();
            }

            handle(EOR_ABY) {
                updateNZ(iStatus, iAccumulator ^= load(aby()));
                size(EOR_ABY);
                dispatch();
            }

            handle(EOR_IX) {
                updateNZ(iStatus, iAccumulator ^= load(ix()));
                size(EOR_IX);
                dispatch();
            }

            handle(EOR_IY) {
                updateNZ(iStatus, iAccumulator ^= load(iy()));
                size(EOR_IY);
                dispatch();
            }

            // Arithmetuc shift left
            handle(ASL_A) {
                iStatus &= ~F_CARRY;
                iStatus |= (iAccumulator & F_NEGATIVE) >> 7; // sign -> carry
                updateNZ(iStatus, iAccumulator <<= 1);
                size(ASL_A);
                dispatch();
            }

            handle(ASL_ZP) {
                aslMemory(oOutside, iStatus, zp());
                size(ASL_ZP);
                dispatch();
            }

            handle(ASL_ZPX) {
                aslMemory(oOutside, iStatus, zpx());
                size(ASL_ZPX);
                dispatch();
            }

            handle(ASL_AB) {
                aslMemory(oOutside, iStatus, ab());
                size(ASL_AB);
                dispatch();
            }

            handle(ASL_ABX) {
                aslMemory(oOutside, iStatus, abx());
                size(ASL_ABX);
                dispatch();
            }

            // Logical shift right
            handle(LSR_A) {
                iStatus &= ~F_CARRY;
                iStatus |= (iAccumulator & F_CARRY);
                updateNZ(iStatus, iAccumulator >>= 1);
                size(LSR_A);
                dispatch();
            }

            handle(ROL_A) {
                iCarry = (iStatus & F_CARRY);
                iStatus &= ~F_CARRY;
                iStatus |= (iAccumulator & F_NEGATIVE) >> 7; // sign -> carry
                updateNZ(iStatus,  iAccumulator = ((iAccumulator << 1) | iCarry) );
                size(ROL_A);
                dispatch();
            }

            handle(ROL_ZP) {
                rolMemory(oOutside, iStatus, zp());
                size(ROL_ZP);
                dispatch();
            }

            handle(ROL_ZPX) {
                rolMemory(oOutside, iStatus, zpx());
                size(ROL_ZPX);
                dispatch();
            }

            handle(ROL_AB) {
                rolMemory(oOutside, iStatus, ab());
                size(ROL_AB);
                dispatch();
            }

            handle(ROL_ABX) {
                rolMemory(oOutside, iStatus, abx());
                size(ROL_ABX);
                dispatch();
            }

            handle(ROR_A) {
                iCarry = (iStatus & F_CARRY) << 7; // carry -> sign
                iStatus &= ~F_CARRY;
                iStatus |= (iAccumulator & F_CARRY); // carry -> carry
                updateNZ(iStatus, iAccumulator = ((iAccumulator >> 1) | iCarry));
                size(ROR_A);
                dispatch();
            }

            handle(ROR_ZP) {
                rorMemory(oOutside, iStatus, zp());
                size(ROR_ZP);
                dispatch();
            }

            handle(ROR_ZPX) {
                rorMemory(oOutside, iStatus, zpx());
                size(ROR_ZPX);
                dispatch();
            }

            handle(ROR_AB) {
                rorMemory(oOutside, iStatus, ab());
                size(ROR_AB);
                dispatch();
            }

            handle(ROR_ABX) {
                rorMemory(oOutside, iStatus, abx());
                size(ROR_ABX);
                dispatch();
            }

            handle(LSR_ZP) {
                lsrMemory(oOutside, iStatus, zp());
                size(LSR_ZP);
                dispatch();
            }

            handle(LSR_ZPX) {
                lsrMemory(oOutside, iStatus, zpx());
                size(LSR_ZPX);
                dispatch();
            }

            handle(LSR_AB) {
                lsrMemory(oOutside, iStatus, ab());
                size(LSR_AB);
                dispatch();
            }

            handle(LSR_ABX) {
                lsrMemory(oOutside, iStatus, abx());
                size(LSR_ABX);
                dispatch();
            }


            // Addition
            // A + M + C
            handle(ADC_IM) {
                addByteWithCarry(iStatus, iAccumulator, load(iProgramCounter + 1));
                size(ADC_IM);
                dispatch();
            }

            handle(ADC_ZP) {
                addByteWithCarry(iStatus, iAccumulator, load(zp()));
                size(ADC_ZP);
                dispatch();
            }

            handle(ADC_ZPX) {
                addByteWithCarry(iStatus, iAccumulator, load(zpx()));
                size(ADC_ZPX);
                dispatch();
            }

            handle(ADC_AB) {
                addByteWithCarry(iStatus, iAccumulator, load(ab()));
                size(ADC_AB);
                dispatch();
            }

            handle(ADC_ABX) {
                addByteWithCarry(iStatus, iAccumulator, load(abx()));
                size(ADC_ABX);
                dispatch();
            }

            handle(ADC_ABY) {
                addByteWithCarry(iStatus, iAccumulator, load(aby()));
                size(ADC_ABY);
                dispatch();
            }

            handle(ADC_IX){
                addByteWithCarry(iStatus, iAccumulator, load(ix()));
                size(ADC_IX);
                dispatch();
            }

            handle(ADC_IY) {
                addByteWithCarry(iStatus, iAccumulator, load(iy()));
                size(ADC_IY);
                dispatch();
            }

            // Subtract
            // A - M - B => A + (255 - M) - (1 - C) => A + ~M + C
            handle(SBC_IM) {
                subByteWithCarry(iStatus, iAccumulator, (load(iProgramCounter + 1)));
                size(SBC_IM);
                dispatch();
            }

            handle(SBC_ZP) {
                subByteWithCarry(iStatus, iAccumulator, (load(zp())));
                size(SBC_ZP);
                dispatch();
            }

            handle(SBC_ZPX) {
                subByteWithCarry(iStatus, iAccumulator, (load(zpx())));
                size(SBC_ZPX);
                dispatch();
            }

            handle(SBC_AB) {
                subByteWithCarry(iStatus, iAccumulator, (load(ab())));
                size(SBC_AB);
                dispatch();
            }

            handle(SBC_ABX) {
                subByteWithCarry(iStatus, iAccumulator, (load(abx())));
                size(SBC_ABX);
                dispatch();
            }

            handle(SBC_ABY) {
                subByteWithCarry(iStatus, iAccumulator, (load(aby())));
                size(SBC_ABY);
                dispatch();
            }

            handle(SBC_IX) {
                subByteWithCarry(iStatus, iAccumulator, (load(ix())));
                size(SBC_IX);
                dispatch();
            }

            handle(SBC_IY) {
                subByteWithCarry(iStatus, iAccumulator, (load(iy())));
                size(SBC_IY);
                dispatch();
            }

            handle(BIT_ZP) {
                iValue = load(zp());
                iStatus &= F_CLR_NZV;
                iStatus |= (iValue & (F_NEGATIVE | F_OVERFLOW)) | (
                    iValue & iAccumulator ? 0 : F_ZERO
                );
                size(BIT_ZP);
                dispatch();
            }

            handle(BIT_AB) {
                iValue = load(ab());
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

                iAddress = readWord(oOutside, iProgramCounter + 1);
                if (iAddress == iProgramCounter) {
                    // Hard Infinite Loop
                    this->iProgramCounter = iProgramCounter; // restore from shadow
                    return *this;
                }
                iProgramCounter = iAddress;
                dispatch();
            }

            handle(JMP_IN) {
                // Emulate the 6502 indirect jump bug with respect to page boundaries.
                Address iPointerAddress = readWord(oOutside, iProgramCounter + 1);
                if (0xFF == (iPointerAddress & 0xFF)) {
                    iAddress = load(iPointerAddress);
                    iAddress |= load(iPointerAddress & 0xFF00) << 8;
                    iProgramCounter = iAddress;
                } else {
                    iProgramCounter = readWord(oOutside, iPointerAddress);
                }
                dispatch();
            }

            handle(JSR_AB) {
                // Note the 6502 notion of the return address is actually the address of the last byte of the operation.
                iAddress = (iProgramCounter + 2);
                push(iAddress >> 8);
                push(iAddress & 0xFF);
                iProgramCounter = readWord(oOutside, iProgramCounter + 1);
                dispatch();
            }

            handle(RTS) {
                iAddress  = pull();
                iAddress |= (pull() << 8);
                iProgramCounter = iAddress + 1;
                dispatch();
            }

            handle(RTI) {
                // Pull SR but ignore bit 5
                iValue = pull() & ~(F_UNUSED|F_BREAK); // clear unused only
                iStatus &= (F_UNUSED|F_BREAK); // clear all but unused flag
                iStatus |= iValue;
                // PC
                iAddress  = pull();
                iAddress |= (pull() << 8);
                iProgramCounter = iAddress;// + 1;
                dispatch();
            }

            handle(BRK) {
                // Push PC+2 as return address
                //iValAddress    = iProgramCounter + 1;
                iAddress = (iProgramCounter + 2);
                push(iAddress >> 8);
                push(iAddress & 0xFF);

                // Push SR
                push(iStatus|F_BREAK|F_UNUSED);

                // Reload PC from IRQ vector
                iProgramCounter = readWord(oOutside, VEC_IRQ);

                // Set interrupted status. Is this the correct location?
                iStatus |= F_INTERRUPT;
                dispatch();
            }

            illegal();
        }
        // Fall through after illegal
        unpin();
        return *this;
    }

#   define INTERNALS_STEP
#endif // nice.
