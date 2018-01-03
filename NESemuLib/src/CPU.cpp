#include "CPU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "LogUtils.h"
#include "MemoryHandler.h"

void CPU::PowerOn()
{
    _programCounter = 0xFFFF;
    _stackPointer = 0xFF;
    _status = 0;
    _accumulator = 0;
    _x = 0;
    _y = 0;
}

void CPU::Reset(MemoryHandler* memoryHandler)
{
    _memoryHandler = memoryHandler;

    // Load program start address in program counter
    _programCounter = _memoryHandler->ReadMem(kResetVectorAddressL) + (_memoryHandler->ReadMem(kResetVectorAddressH) << 8) - 1;
    
    // https://stackoverflow.com/questions/16913423/why-is-the-initial-state-of-the-interrupt-flag-of-the-6502-a-1
    SetFlag(Flag::InterruptDisable, true);
}

void CPU::Tick()
{
    --_ticksUntilNextInstruction;
    if (_ticksUntilNextInstruction <= 0)
    {
        _ticksUntilNextInstruction = ExecuteNextInstruction() - 1;
    }
}

int CPU::ExecuteNextInstruction()
{
    // Fetch next opcode
    const uint8_t opcode = _memoryHandler->ReadMem(++_programCounter);
    //Log::Debug("Executing opcode: $%02X", opcode);

    switch (opcode)
    {
        // LDA
        case 0xA9: return LDA(AddressingMode::Immediate);
        case 0xA5: return LDA(AddressingMode::ZeroPage);
        case 0xB5: return LDA(AddressingMode::ZeroPageX);
        case 0xAD: return LDA(AddressingMode::Absolute);
        case 0xBD: return LDA(AddressingMode::AbsoluteX);
        case 0xB9: return LDA(AddressingMode::AbsoluteY);
        case 0xA1: return LDA(AddressingMode::IndirectX);
        case 0xB1: return LDA(AddressingMode::IndirectY);

        // LDX
        case 0xA2: return LDX(AddressingMode::Immediate);
        case 0xA6: return LDX(AddressingMode::ZeroPage);
        case 0xB6: return LDX(AddressingMode::ZeroPageY);
        case 0xAE: return LDX(AddressingMode::Absolute);
        case 0xBE: return LDX(AddressingMode::AbsoluteY);

        // LDY
        case 0xA0: return LDY(AddressingMode::Immediate);
        case 0xA4: return LDY(AddressingMode::ZeroPage);
        case 0xB4: return LDY(AddressingMode::ZeroPageX);
        case 0xAC: return LDY(AddressingMode::Absolute);
        case 0xBC: return LDY(AddressingMode::AbsoluteX);

        // STA
        case 0x85: return STA(AddressingMode::ZeroPage);
        case 0x95: return STA(AddressingMode::ZeroPageX);
        case 0x8D: return STA(AddressingMode::Absolute);
        case 0x9D: return STA(AddressingMode::AbsoluteX);
        case 0x99: return STA(AddressingMode::AbsoluteY);
        case 0x81: return STA(AddressingMode::IndirectX);
        case 0x91: return STA(AddressingMode::IndirectY);

        // STX
        case 0x86: return STX(AddressingMode::ZeroPage);
        case 0x96: return STX(AddressingMode::ZeroPageY);
        case 0x8E: return STX(AddressingMode::Absolute);

        // STY
        case 0x84: return STY(AddressingMode::ZeroPage);
        case 0x94: return STY(AddressingMode::ZeroPageX);
        case 0x8C: return STY(AddressingMode::Absolute);

        // ADC
        case 0x69: return ADC(AddressingMode::Immediate);
        case 0x65: return ADC(AddressingMode::ZeroPage);
        case 0x75: return ADC(AddressingMode::ZeroPageX);
        case 0x6D: return ADC(AddressingMode::Absolute);
        case 0x7D: return ADC(AddressingMode::AbsoluteX);
        case 0x79: return ADC(AddressingMode::AbsoluteY);
        case 0x61: return ADC(AddressingMode::IndirectX);
        case 0x71: return ADC(AddressingMode::IndirectY);

        // SBC
        case 0xE9: return SBC(AddressingMode::Immediate);
        case 0xE5: return SBC(AddressingMode::ZeroPage);
        case 0xF5: return SBC(AddressingMode::ZeroPageX);
        case 0xED: return SBC(AddressingMode::Absolute);
        case 0xFD: return SBC(AddressingMode::AbsoluteX);
        case 0xF9: return SBC(AddressingMode::AbsoluteY);
        case 0xE1: return SBC(AddressingMode::IndirectX);
        case 0xF1: return SBC(AddressingMode::IndirectY);

        // CPU Flags
        case 0x18: return CLC(AddressingMode::Implied);
        case 0x38: return SEC(AddressingMode::Implied);
        case 0x58: return CLI(AddressingMode::Implied);
        case 0x78: return SEI(AddressingMode::Implied);
        case 0xB8: return CLV(AddressingMode::Implied);
        case 0xD8: return CLD(AddressingMode::Implied);
        case 0xF8: return SED(AddressingMode::Implied);

        // Increment/Decrement
        case 0xCA: return DEX(AddressingMode::Implied);
        case 0xE8: return INX(AddressingMode::Implied);
        case 0x88: return DEY(AddressingMode::Implied);
        case 0xC8: return INY(AddressingMode::Implied);
        case 0xC6: return DECINC(AddressingMode::ZeroPage, -1);
        case 0xD6: return DECINC(AddressingMode::ZeroPageX, -1);
        case 0xCE: return DECINC(AddressingMode::Absolute, -1);
        case 0xDE: return DECINC(AddressingMode::AbsoluteX, -1);
        case 0xE6: return DECINC(AddressingMode::ZeroPage, +1);
        case 0xF6: return DECINC(AddressingMode::ZeroPageX, +1);
        case 0xEE: return DECINC(AddressingMode::Absolute, +1);
        case 0xFE: return DECINC(AddressingMode::AbsoluteX, +1);

        // Routine instructions
        case 0x4C: return JMP(AddressingMode::Absolute);
        case 0x6C: return JMP(AddressingMode::Indirect);
        case 0x20: return JSR(AddressingMode::Absolute);
        case 0x60: return RTS(AddressingMode::Implied);
        case 0x40: return RTI(AddressingMode::Implied);

        // Compare instructions
        case 0xC9: return CMP(AddressingMode::Immediate, _accumulator); // CMP
        case 0xC5: return CMP(AddressingMode::ZeroPage, _accumulator);  // CMP
        case 0xD5: return CMP(AddressingMode::ZeroPageX, _accumulator); // CMP
        case 0xCD: return CMP(AddressingMode::Absolute, _accumulator);  // CMP
        case 0xDD: return CMP(AddressingMode::AbsoluteX, _accumulator); // CMP
        case 0xD9: return CMP(AddressingMode::AbsoluteY, _accumulator); // CMP
        case 0xC1: return CMP(AddressingMode::IndirectX, _accumulator); // CMP
        case 0xD1: return CMP(AddressingMode::IndirectY, _accumulator); // CMP
        case 0xE0: return CMP(AddressingMode::Immediate, _x);           // CPX
        case 0xE4: return CMP(AddressingMode::ZeroPage, _x);            // CPX
        case 0xEC: return CMP(AddressingMode::Absolute, _x);            // CPX
        case 0xC0: return CMP(AddressingMode::Immediate, _y);           // CPY
        case 0xC4: return CMP(AddressingMode::ZeroPage, _y);            // CPY
        case 0xCC: return CMP(AddressingMode::Absolute, _y);            // CPY

        // Logic instructions
        case 0x24: return BIT(AddressingMode::ZeroPage);
        case 0x2C: return BIT(AddressingMode::Absolute);
        case 0x29: return AND(AddressingMode::Immediate);
        case 0x25: return AND(AddressingMode::ZeroPage);
        case 0x35: return AND(AddressingMode::ZeroPageX);
        case 0x2D: return AND(AddressingMode::Absolute);
        case 0x3D: return AND(AddressingMode::AbsoluteX);
        case 0x39: return AND(AddressingMode::AbsoluteY);
        case 0x21: return AND(AddressingMode::IndirectX);
        case 0x31: return AND(AddressingMode::IndirectX);
        case 0x09: return ORA(AddressingMode::Immediate);
        case 0x05: return ORA(AddressingMode::ZeroPage);
        case 0x15: return ORA(AddressingMode::ZeroPageX);
        case 0x0D: return ORA(AddressingMode::Absolute);
        case 0x1D: return ORA(AddressingMode::AbsoluteX);
        case 0x19: return ORA(AddressingMode::AbsoluteY);
        case 0x01: return ORA(AddressingMode::IndirectX);
        case 0x11: return ORA(AddressingMode::IndirectX);
        case 0x49: return EOR(AddressingMode::Immediate);
        case 0x45: return EOR(AddressingMode::ZeroPage);
        case 0x55: return EOR(AddressingMode::ZeroPageX);
        case 0x4D: return EOR(AddressingMode::Absolute);
        case 0x5D: return EOR(AddressingMode::AbsoluteX);
        case 0x59: return EOR(AddressingMode::AbsoluteY);
        case 0x41: return EOR(AddressingMode::IndirectX);
        case 0x51: return EOR(AddressingMode::IndirectX);

        // Shift instructions
        case 0x0A: return ASL(AddressingMode::Accumulator);
        case 0x06: return ASL(AddressingMode::ZeroPage);
        case 0x16: return ASL(AddressingMode::ZeroPageX);
        case 0x0E: return ASL(AddressingMode::Absolute);
        case 0x1E: return ASL(AddressingMode::AbsoluteX);
        case 0x4A: return LSR(AddressingMode::Accumulator);
        case 0x46: return LSR(AddressingMode::ZeroPage);
        case 0x56: return LSR(AddressingMode::ZeroPageX);
        case 0x4E: return LSR(AddressingMode::Absolute);
        case 0x5E: return LSR(AddressingMode::AbsoluteX);
        case 0x2A: return ROL(AddressingMode::Accumulator);
        case 0x26: return ROL(AddressingMode::ZeroPage);
        case 0x36: return ROL(AddressingMode::ZeroPageX);
        case 0x2E: return ROL(AddressingMode::Absolute);
        case 0x3E: return ROL(AddressingMode::AbsoluteX);
        case 0x6A: return ROR(AddressingMode::Accumulator);
        case 0x66: return ROR(AddressingMode::ZeroPage);
        case 0x76: return ROR(AddressingMode::ZeroPageX);
        case 0x6E: return ROR(AddressingMode::Absolute);
        case 0x7E: return ROR(AddressingMode::AbsoluteX);

        // Branch-on-condition instructions
        case 0xF0: return BXX(AddressingMode::Relative, GetFlag(Flag::Zero));      // BEQ
        case 0xD0: return BXX(AddressingMode::Relative, !GetFlag(Flag::Zero));     // BNE
        case 0xB0: return BXX(AddressingMode::Relative, GetFlag(Flag::Carry));     // BCS
        case 0x90: return BXX(AddressingMode::Relative, !GetFlag(Flag::Carry));    // BCC
        case 0x70: return BXX(AddressingMode::Relative, GetFlag(Flag::Overflow));  // BVS
        case 0x50: return BXX(AddressingMode::Relative, !GetFlag(Flag::Overflow)); // BVC
        case 0x30: return BXX(AddressingMode::Relative, GetFlag(Flag::Sign));      // BMI
        case 0x10: return BXX(AddressingMode::Relative, !GetFlag(Flag::Sign));     // BPL

        // Transfer instructions
        case 0x9A: return TXX(AddressingMode::Implied, _x, _stackPointer); // TXS
        case 0xBA: return TXX(AddressingMode::Implied, _stackPointer, _x); // TSX
        case 0xAA: return TXX(AddressingMode::Implied, _accumulator, _x);  // TAX
        case 0x8A: return TXX(AddressingMode::Implied, _x, _accumulator);  // TXA
        case 0xA8: return TXX(AddressingMode::Implied, _accumulator, _y);  // TAY
        case 0x98: return TXX(AddressingMode::Implied, _y, _accumulator);  // TYA
        case 0x48: return PHA(AddressingMode::Implied);                    // PHA
        case 0x68: return PLA(AddressingMode::Implied);                    // PLA
        case 0x08: return PHP(AddressingMode::Implied);                    // PHP
        case 0x28: return PLP(AddressingMode::Implied);                    // PLP

        // Other
        case 0x00: return BRK(AddressingMode::Implied);
        case 0xEA: return NOP(AddressingMode::Implied);

        default: Log::Debug("Unknown opcode: $%02X", opcode); return 0;
    };
}

void CPU::ExecuteNMI()
{
    const uint8_t statusPlusBFlag = _status | (1 << Flag::Break) | (1 << Flag::Unused);
    Push(GetHighByte(_programCounter));
    Push(GetLowByte(_programCounter));
    Push(statusPlusBFlag);
    SetFlag(Flag::InterruptDisable, true);
    _programCounter = _memoryHandler->ReadMem(kNMIVectorAddressL) +
        (_memoryHandler->ReadMem(kNMIVectorAddressH) << 8) - 1;
}

void CPU::SetAccumulator(uint8_t value)
{
    _accumulator = value;
    SetFlag(Flag::Sign, IsValueNegative(_accumulator));
    SetFlag(Flag::Zero, _accumulator == 0);
}

void CPU::SetFlag(Flag flag, bool value)
{
    if (flag == Flag::Unused || flag == Flag::Break)
    {
        OMBAssert(false, "Can't set flag %d", flag);
        return;
    }

    BitwiseUtils::SetFlag(_status, flag, value);
}

bool CPU::GetFlag(Flag flag) const
{
    if (flag == Flag::Unused)
    {
        OMBAssert(false, "Unused flag only exists in the stack!");
        return true;
    }
    else if (flag == Flag::Break)
    {
        OMBAssert(false, "Break flag only exists in the stack!");
        return false;
    }
    else
    {
        return BitwiseUtils::IsFlagSet(_status, flag);
    }
}

uint8_t CPU::PeekStack(int index)
{
    return _memoryHandler->ReadMem(kStackAddressStart + _stackPointer + 1 + index);
}

uint8_t CPU::GetValueWithMode(AddressingMode mode, int& cycles)
{
    uint8_t value = 0;
    switch (mode)
    {
        case AddressingMode::Accumulator:
        {
            value = _accumulator;
            cycles += 0; // Not sure about this one. Only used for shift and rotate instructions.
            break;
        }
        case AddressingMode::Immediate:
        {
            value = _memoryHandler->ReadMem(++_programCounter);
            cycles += 1;
            break;
        }
        case AddressingMode::ZeroPage:
        {
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter);
            value = _memoryHandler->ReadMem(address);
            cycles += 2;
            break;
        }
        case AddressingMode::ZeroPageX:
        case AddressingMode::ZeroPageY:
        {
            const uint8_t regValue = (mode == AddressingMode::ZeroPageX) ? _x : _y;
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter) + regValue;
            value = _memoryHandler->ReadMem(address);
            cycles += 3;
            break;
        }
        case AddressingMode::Absolute:
        {
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter) + (_memoryHandler->ReadMem(++_programCounter) << 8);
            value = _memoryHandler->ReadMem(address);
            cycles += 3;
            break;
        }
        case AddressingMode::AbsoluteX:
        case AddressingMode::AbsoluteY:
        {
            const uint8_t regValue = (mode == AddressingMode::AbsoluteX) ? _x : _y;
            const uint8_t loByte = _memoryHandler->ReadMem(++_programCounter);
            const uint16_t loBytePlusReg = loByte + regValue;
            const uint8_t hiByte = _memoryHandler->ReadMem(++_programCounter);
            const uint16_t address = loBytePlusReg + (hiByte << 8);
            value = _memoryHandler->ReadMem(address);
            cycles += 3;
            if (loBytePlusReg != (uint8_t)loBytePlusReg)
            {
                // If page boundary crossed when adding low byte and register, it takes 1 more cycle due to carry
                cycles += 1;
            }
            break;
        }
        case AddressingMode::IndirectX:
        {
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter) + _x;
            const uint16_t finalAddress = _memoryHandler->ReadMem(address) + (_memoryHandler->ReadMem(address + 1) << 8);
            value = _memoryHandler->ReadMem(finalAddress);
            cycles += 5;
            break;
        }
        case AddressingMode::IndirectY:
        {
            const uint8_t address = _memoryHandler->ReadMem(++_programCounter);
            const uint8_t loByte = _memoryHandler->ReadMem(address);
            const uint16_t loBytePlusReg = loByte + _y;
            const uint8_t hiByte = _memoryHandler->ReadMem(address + 1);
            const uint16_t finalAddress = loBytePlusReg + (hiByte << 8);
            value = _memoryHandler->ReadMem(finalAddress);
            cycles += 4;
            if (loBytePlusReg != (uint8_t)loBytePlusReg)
            {
                cycles += 1;
            }
            break;
        }
        default:
        {
            OMBAssert(false, "Can't get value with addressingMode %d", mode);
        }
    }

    return value;
}

void CPU::SetValueWithMode(AddressingMode mode, uint8_t value, int& cycles)
{
    switch (mode)
    {
        case AddressingMode::ZeroPage:
        {
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter);
            _memoryHandler->WriteMem(address, value);
            cycles += 2;
            break;
        }
        case AddressingMode::ZeroPageX:
        case AddressingMode::ZeroPageY:
        {
            const uint8_t regValue = (mode == AddressingMode::ZeroPageX) ? _x : _y;
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter) + regValue;
            _memoryHandler->WriteMem(address, value);
            cycles += 3;
            break;
        }
        case AddressingMode::Absolute:
        {
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter) + (_memoryHandler->ReadMem(++_programCounter) << 8);
            _memoryHandler->WriteMem(address, value);
            cycles += 3;
            break;
        }
        case AddressingMode::AbsoluteX:
        case AddressingMode::AbsoluteY:
        {
            const uint8_t regValue = (mode == AddressingMode::AbsoluteX) ? _x : _y;
            const uint8_t loByte = _memoryHandler->ReadMem(++_programCounter);
            const uint16_t loBytePlusReg = loByte + regValue;
            const uint8_t hiByte = _memoryHandler->ReadMem(++_programCounter);
            const uint16_t address = loBytePlusReg + (hiByte << 8);
            _memoryHandler->WriteMem(address, value);
            cycles += 4;
            break;
        }
        case AddressingMode::IndirectX:
        {
            const uint16_t address = _memoryHandler->ReadMem(++_programCounter) + _x;
            const uint16_t finalAddress = _memoryHandler->ReadMem(address) + (_memoryHandler->ReadMem(address + 1) << 8);
            _memoryHandler->WriteMem(finalAddress, value);
            cycles += 5;
            break;
        }
        case AddressingMode::IndirectY:
        {
            const uint8_t address = _memoryHandler->ReadMem(++_programCounter);
            const uint8_t loByte = _memoryHandler->ReadMem(address);
            const uint16_t loBytePlusReg = loByte + _y;
            const uint8_t hiByte = _memoryHandler->ReadMem(address + 1);
            const uint16_t finalAddress = loBytePlusReg + (hiByte << 8);
            _memoryHandler->WriteMem(finalAddress, value);
            cycles += 5;
            break;
        }
        default:
        {
            OMBAssert(false, "Can't set value with addressingMode %d", mode);
        }
    }
}

void CPU::Push(uint8_t value)
{
    _memoryHandler->WriteMem(kStackAddressStart + _stackPointer, value);
    OMBAssert(_stackPointer > 0, "Stack overflow!");
    --_stackPointer;
}

uint8_t CPU::Pop()
{
    OMBAssert(_stackPointer < 0xFF, "Stack overflow!");
    ++_stackPointer;
    return _memoryHandler->ReadMem(kStackAddressStart + _stackPointer);
}

uint8_t CPU::GetLowByte(uint16_t value) const
{
    return 0x00FF & value;
}

uint8_t CPU::GetHighByte(uint16_t value) const
{
    return value >> 8;
}

bool CPU::IsValueNegative(uint8_t value) const
{
    return (value >> 7) > 0;
}
