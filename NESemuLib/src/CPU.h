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
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        IndirectX,
        IndirectY
    };

    void PowerOn();
    void Reset(MemoryHandler* memoryHandler);

    int ExecuteNextInstruction(); // Returns number of CPU cycles taken

    // For testing purposes
    uint8_t GetAccumulatorValue() { return _accumulator; };
    uint8_t GetXValue() { return _x; };
    uint8_t GetYValue() { return _y; };
    bool GetFlag(Flag flag);

private:
    uint16_t _programCounter;
    uint8_t _stackPointer;
    uint8_t _status;
    uint8_t _accumulator;
    uint8_t _x;
    uint8_t _y;
    std::map<uint8_t, std::function<int()>> _opcodes = {
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

        // CPU Flags
        { 0x18, [this]() -> int { return CLC(AddressingMode::Implied); } },
        { 0x38, [this]() -> int { return SEC(AddressingMode::Implied); } },
        { 0xB8, [this]() -> int { return CLV(AddressingMode::Implied); } },
    };

    MemoryHandler* _memoryHandler = nullptr;

    void SetFlag(Flag flag, bool value);
    uint8_t GetValueWithMode(AddressingMode mode, int& cycles);
    void SetValueWithMode(AddressingMode mode, uint8_t value, int& cycles);
    bool IsValueNegative(uint8_t value) const;

    int LDA(AddressingMode mode);
    int LDX(AddressingMode mode);
    int LDY(AddressingMode mode);
    
    int STA(AddressingMode mode);
    int STX(AddressingMode mode);
    int STY(AddressingMode mode);

    int ADC(AddressingMode mode);

    int CLC(AddressingMode mode);
    int SEC(AddressingMode mode);
    int CLV(AddressingMode mode);
};
