#pragma once

#include <functional>
#include <map>
#include <stdint.h>

class MemoryHandler;

class CPU
{
public:
    static const int kStackSize = 256;
    static const uint16_t kStackAddressStart = 0x100;
    static const uint16_t kResetVectorAddressL = 0xFFFC;
    static const uint16_t kResetVectorAddressH = kResetVectorAddressL + 1;
    static const uint16_t kInterruptBreakVectorAddressL = 0xFFFE;
    static const uint16_t kInterruptBreakVectorAddressH = kInterruptBreakVectorAddressL + 1;

    enum Flag
    {
        Carry = 0,          // C
        Zero,               // Z
        InterruptDisable,   // I
        DecimalMode,        // D
        Break,              // B
        Unused,             // 1
        Overflow,           // V
        Sign                // S (1 for Negative)
    };

    enum AddressingMode
    {
        Implied,
        Relative,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        Indirect,
        IndirectX,
        IndirectY
    };

    void PowerOn();
    void Reset(MemoryHandler* memoryHandler);

    int ExecuteNextInstruction(); // Returns number of CPU cycles taken

    // Internal or for testing purposes (hence the publicness) Don't use these without a good reason
    uint16_t GetProgramCounter() const { return _programCounter; }
    void SetProgramCounter(uint16_t value) { _programCounter = value; }
    uint8_t GetStatus() const { return _status; }
    uint8_t GetAccumulator() const { return _accumulator; }
    uint8_t GetX() const { return _x; }
    uint8_t GetY() const { return _y; }
    void SetAccumulator(uint8_t value);
    void SetX(uint8_t value) { _x = value; }
    void SetY(uint8_t value) { _y = value; }
    void SetFlag(Flag flag, bool value);
    bool GetFlag(Flag flag) const;
    uint8_t PeekStack(int index);
    uint8_t GetStackPointer() const { return _stackPointer; }
    void SetStackPointer(uint8_t value) { _stackPointer = value; }

private:
    uint16_t _programCounter;
    uint8_t _stackPointer;
    uint8_t _status;
    uint8_t _accumulator;
    uint8_t _x;
    uint8_t _y;
    const std::map<uint8_t, std::function<int()>> _opcodes = {
        // LDA
        { 0xA9, [this]() -> int { return LDA(AddressingMode::Immediate); } },
        { 0xA5, [this]() -> int { return LDA(AddressingMode::ZeroPage); } },
        { 0xB5, [this]() -> int { return LDA(AddressingMode::ZeroPageX); } },
        { 0xAD, [this]() -> int { return LDA(AddressingMode::Absolute); } },
        { 0xBD, [this]() -> int { return LDA(AddressingMode::AbsoluteX); } },
        { 0xB9, [this]() -> int { return LDA(AddressingMode::AbsoluteY); } },
        { 0xA1, [this]() -> int { return LDA(AddressingMode::IndirectX); } },
        { 0xB1, [this]() -> int { return LDA(AddressingMode::IndirectY); } },

        // LDX
        { 0xA2, [this]() -> int { return LDX(AddressingMode::Immediate); } },
        { 0xA6, [this]() -> int { return LDX(AddressingMode::ZeroPage); } },
        { 0xB6, [this]() -> int { return LDX(AddressingMode::ZeroPageY); } },
        { 0xAE, [this]() -> int { return LDX(AddressingMode::Absolute); } },
        { 0xBE, [this]() -> int { return LDX(AddressingMode::AbsoluteY); } },

        // LDY
        { 0xA0, [this]() -> int { return LDY(AddressingMode::Immediate); } },
        { 0xA4, [this]() -> int { return LDY(AddressingMode::ZeroPage); } },
        { 0xB4, [this]() -> int { return LDY(AddressingMode::ZeroPageX); } },
        { 0xAC, [this]() -> int { return LDY(AddressingMode::Absolute); } },
        { 0xBC, [this]() -> int { return LDY(AddressingMode::AbsoluteX); } },

        // STA
        { 0x85, [this]() -> int { return STA(AddressingMode::ZeroPage); } },
        { 0x95, [this]() -> int { return STA(AddressingMode::ZeroPageX); } },
        { 0x8D, [this]() -> int { return STA(AddressingMode::Absolute); } },
        { 0x9D, [this]() -> int { return STA(AddressingMode::AbsoluteX); } },
        { 0x99, [this]() -> int { return STA(AddressingMode::AbsoluteY); } },
        { 0x81, [this]() -> int { return STA(AddressingMode::IndirectX); } },
        { 0x91, [this]() -> int { return STA(AddressingMode::IndirectY); } },

        // STX
        { 0x86, [this]() -> int { return STX(AddressingMode::ZeroPage); } },
        { 0x96, [this]() -> int { return STX(AddressingMode::ZeroPageY); } },
        { 0x8E, [this]() -> int { return STX(AddressingMode::Absolute); } },

        // STY
        { 0x84, [this]() -> int { return STY(AddressingMode::ZeroPage); } },
        { 0x94, [this]() -> int { return STY(AddressingMode::ZeroPageX); } },
        { 0x8C, [this]() -> int { return STY(AddressingMode::Absolute); } },

        // ADC
        { 0x69, [this]() -> int { return ADC(AddressingMode::Immediate); } },
        { 0x65, [this]() -> int { return ADC(AddressingMode::ZeroPage); } },
        { 0x75, [this]() -> int { return ADC(AddressingMode::ZeroPageX); } },
        { 0x6D, [this]() -> int { return ADC(AddressingMode::Absolute); } },
        { 0x7D, [this]() -> int { return ADC(AddressingMode::AbsoluteX); } },
        { 0x79, [this]() -> int { return ADC(AddressingMode::AbsoluteY); } },
        { 0x61, [this]() -> int { return ADC(AddressingMode::IndirectX); } },
        { 0x71, [this]() -> int { return ADC(AddressingMode::IndirectY); } },

        // SBC
        { 0xE9, [this]() -> int { return SBC(AddressingMode::Immediate); } },
        { 0xE5, [this]() -> int { return SBC(AddressingMode::ZeroPage); } },
        { 0xF5, [this]() -> int { return SBC(AddressingMode::ZeroPageX); } },
        { 0xED, [this]() -> int { return SBC(AddressingMode::Absolute); } },
        { 0xFD, [this]() -> int { return SBC(AddressingMode::AbsoluteX); } },
        { 0xF9, [this]() -> int { return SBC(AddressingMode::AbsoluteY); } },
        { 0xE1, [this]() -> int { return SBC(AddressingMode::IndirectX); } },
        { 0xF1, [this]() -> int { return SBC(AddressingMode::IndirectY); } },

        // CPU Flags
        { 0x18, [this]() -> int { return CLC(AddressingMode::Implied); } },
        { 0x38, [this]() -> int { return SEC(AddressingMode::Implied); } },
        { 0x58, [this]() -> int { return CLI(AddressingMode::Implied); } },
        { 0x78, [this]() -> int { return SEI(AddressingMode::Implied); } },
        { 0xB8, [this]() -> int { return CLV(AddressingMode::Implied); } },
        { 0xD8, [this]() -> int { return CLD(AddressingMode::Implied); } },
        { 0xF8, [this]() -> int { return SED(AddressingMode::Implied); } },

        // Increment/Decrement
        { 0xCA, [this]() -> int { return DEX(AddressingMode::Implied); } },
        { 0xE8, [this]() -> int { return INX(AddressingMode::Implied); } },
        { 0x88, [this]() -> int { return DEY(AddressingMode::Implied); } },
        { 0xC8, [this]() -> int { return INY(AddressingMode::Implied); } },
        { 0xC6, [this]() -> int { return DECINC(AddressingMode::ZeroPage, -1); } },
        { 0xD6, [this]() -> int { return DECINC(AddressingMode::ZeroPageX, -1); } },
        { 0xCE, [this]() -> int { return DECINC(AddressingMode::Absolute, -1); } },
        { 0xDE, [this]() -> int { return DECINC(AddressingMode::AbsoluteX, -1); } },
        { 0xE6, [this]() -> int { return DECINC(AddressingMode::ZeroPage, +1); } },
        { 0xF6, [this]() -> int { return DECINC(AddressingMode::ZeroPageX, +1); } },
        { 0xEE, [this]() -> int { return DECINC(AddressingMode::Absolute, +1); } },
        { 0xFE, [this]() -> int { return DECINC(AddressingMode::AbsoluteX, +1); } },

        // Routine instructions
        { 0x4C, [this]() -> int { return JMP(AddressingMode::Absolute); } },
        { 0x6C, [this]() -> int { return JMP(AddressingMode::Indirect); } },
        { 0x20, [this]() -> int { return JSR(AddressingMode::Absolute); } },
        { 0x60, [this]() -> int { return RTS(AddressingMode::Implied); } },
        { 0x40, [this]() -> int { return RTI(AddressingMode::Implied); } },

        // Compare instructions
        { 0xC9, [this]() -> int { return CMP(AddressingMode::Immediate, _accumulator); } },
        { 0xC5, [this]() -> int { return CMP(AddressingMode::ZeroPage, _accumulator); } },
        { 0xD5, [this]() -> int { return CMP(AddressingMode::ZeroPageX, _accumulator); } },
        { 0xCD, [this]() -> int { return CMP(AddressingMode::Absolute, _accumulator); } },
        { 0xDD, [this]() -> int { return CMP(AddressingMode::AbsoluteX, _accumulator); } },
        { 0xD9, [this]() -> int { return CMP(AddressingMode::AbsoluteY, _accumulator); } },
        { 0xC1, [this]() -> int { return CMP(AddressingMode::IndirectX, _accumulator); } },
        { 0xD1, [this]() -> int { return CMP(AddressingMode::IndirectY, _accumulator); } },

        // Branch-on-condition instructions
        { 0xF0, [this]() -> int { return BXX(AddressingMode::Relative, GetFlag(Flag::Zero)); } },      // BEQ
        { 0xD0, [this]() -> int { return BXX(AddressingMode::Relative, !GetFlag(Flag::Zero)); } },     // BNE
        { 0xB0, [this]() -> int { return BXX(AddressingMode::Relative, GetFlag(Flag::Carry)); } },     // BCS
        { 0x90, [this]() -> int { return BXX(AddressingMode::Relative, !GetFlag(Flag::Carry)); } },    // BCC
        { 0x70, [this]() -> int { return BXX(AddressingMode::Relative, GetFlag(Flag::Overflow)); } },  // BVS
        { 0x50, [this]() -> int { return BXX(AddressingMode::Relative, !GetFlag(Flag::Overflow)); } }, // BVC
        { 0x30, [this]() -> int { return BXX(AddressingMode::Relative, GetFlag(Flag::Sign)); } },      // BMI
        { 0x10, [this]() -> int { return BXX(AddressingMode::Relative, !GetFlag(Flag::Sign)); } },     // BPL

        // Transfer instructions
        { 0x9A, [this]() -> int { return TXX(AddressingMode::Implied, _x, _stackPointer); } }, // TXS
        { 0xBA, [this]() -> int { return TXX(AddressingMode::Implied, _stackPointer, _x); } }, // TSX
        { 0xAA, [this]() -> int { return TXX(AddressingMode::Implied, _accumulator, _x); } },  // TAX
        { 0x8A, [this]() -> int { return TXX(AddressingMode::Implied, _x, _accumulator); } },  // TXA
        { 0xA8, [this]() -> int { return TXX(AddressingMode::Implied, _accumulator, _y); } },  // TAY
        { 0x98, [this]() -> int { return TXX(AddressingMode::Implied, _y, _accumulator); } },  // TYA

        // Other
        { 0x00, [this]() -> int { return BRK(AddressingMode::Implied); } },
    };

    MemoryHandler* _memoryHandler = nullptr;

    uint8_t GetValueWithMode(AddressingMode mode, int& cycles);
    void SetValueWithMode(AddressingMode mode, uint8_t value, int& cycles);
    void Push(uint8_t value);
    uint8_t Pop();

    uint8_t GetLowByte(uint16_t value) const;
    uint8_t GetHighByte(uint16_t value) const;
    bool IsValueNegative(uint8_t value) const;

    int LDA(AddressingMode mode);
    int LDX(AddressingMode mode);
    int LDY(AddressingMode mode);
    
    int STA(AddressingMode mode);
    int STX(AddressingMode mode);
    int STY(AddressingMode mode);

    int ADC(AddressingMode mode);
    int SBC(AddressingMode mode);

    int CLC(AddressingMode mode);
    int SEC(AddressingMode mode);
    int CLI(AddressingMode mode);
    int SEI(AddressingMode mode);
    int CLV(AddressingMode mode);
    int CLD(AddressingMode mode);
    int SED(AddressingMode mode);

    int DEX(AddressingMode mode);
    int INX(AddressingMode mode);
    int DEY(AddressingMode mode);
    int INY(AddressingMode mode);
    int DECINC(AddressingMode mode, int delta);

    int JMP(AddressingMode mode);
    int JSR(AddressingMode mode);
    int RTS(AddressingMode mode);
    int RTI(AddressingMode mode);

    int CMP(AddressingMode mode, uint8_t registerValue); // Used for all compare instructions

    int BXX(AddressingMode mode, bool condition); // Used for all branch instructions

    int TXX(AddressingMode mode, uint8_t from, uint8_t& to); // Used for all transfer instructions

    int BRK(AddressingMode mode);
};
