#include "CPU.h"

#include "Assert.h"
#include "LogUtils.h"
#include "MemoryHandler.h"

void CPU::PowerOn()
{
	_programCounter = 0;
	_stackPointer = 0;
	_status = 0;
	_accumulator = 0;
	_x = 0;
	_y = 0;

	SetFlag(Flag::Unused, true);
}

void CPU::Reset(MemoryHandler* memoryHandler)
{
	_memoryHandler = memoryHandler;

	// Load program start address in program counter
	int temp1 = _memoryHandler->ReadMem(kResetVectorAddressH);
	int temp2 = _memoryHandler->ReadMem(kResetVectorAddressL);

	_programCounter = _memoryHandler->ReadMem(kResetVectorAddressH) << 8;
	_programCounter += _memoryHandler->ReadMem(kResetVectorAddressL);
}

int CPU::ExecuteNextInstruction()
{
	// Fetch next opcode
	uint8_t opcode = _memoryHandler->ReadMem(_programCounter++);
	int cycles = 0;

	if (_opcodes.find(opcode) != _opcodes.end())
	{
		Log::Debug("Executing opcode: $%04X", opcode);
		cycles = _opcodes[opcode]();
		Log::Debug("Took %d cycles", cycles);
	}
	else
	{
		Log::Debug("Unknown opcode: $%04X", opcode);
	}

	return cycles;
}

void CPU::SetFlag(Flag flag, bool value)
{
	if (value)
	{
		_status |= 1 << flag;
	}
	else
	{
		_status &= ~(1 << flag);
	}
}

bool CPU::GetFlag(Flag flag)
{
	return (_status & (1 << flag)) != 0;
}

uint8_t CPU::GetValueWithMode(AddressingMode mode, int& cycles)
{
	uint8_t value = 0;
	switch (mode)
	{
		case AddressingMode::Immediate:
		{
			value = _memoryHandler->ReadMem(_programCounter++);
			cycles += 1;
			break;
		}
		case AddressingMode::ZeroPage:
		{
			const uint16_t address = _memoryHandler->ReadMem(_programCounter++);
			value = _memoryHandler->ReadMem(address);
			cycles += 2;
			break;
		}
		case AddressingMode::ZeroPageX:
		case AddressingMode::ZeroPageY:
		{
			const uint8_t regValue = (mode == AddressingMode::ZeroPageX) ? _x : _y;
			const uint16_t address = _memoryHandler->ReadMem(_programCounter++) + regValue;
			value = _memoryHandler->ReadMem(address);
			cycles += 3;
			break;
		}
		case AddressingMode::Absolute:
		{
			const uint16_t address = _memoryHandler->ReadMem(_programCounter++) + (_memoryHandler->ReadMem(_programCounter++) << 8);
			value = _memoryHandler->ReadMem(address);
			cycles += 3;
			break;
		}
		case AddressingMode::AbsoluteX:
		case AddressingMode::AbsoluteY:
		{
			const uint8_t regValue = (mode == AddressingMode::AbsoluteX) ? _x : _y;
			const uint8_t loByte = _memoryHandler->ReadMem(_programCounter++);
			const uint16_t loBytePlusReg = loByte + regValue;
			const uint8_t hiByte = _memoryHandler->ReadMem(_programCounter++);
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
			const uint16_t address = _memoryHandler->ReadMem(_programCounter++) + _x;
			const uint16_t finalAddress = _memoryHandler->ReadMem(address) + (_memoryHandler->ReadMem(address + 1) << 8);
			value = _memoryHandler->ReadMem(finalAddress);
			cycles += 5;
			break;
		}
		case AddressingMode::IndirectY:
		{
			const uint8_t address = _memoryHandler->ReadMem(_programCounter++);
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
            const uint16_t address = _memoryHandler->ReadMem(_programCounter++);
            _memoryHandler->WriteMem(address, value);
            cycles += 2;
            break;
        }
        case AddressingMode::ZeroPageX:
        case AddressingMode::ZeroPageY:
        {
            const uint8_t regValue = (mode == AddressingMode::ZeroPageX) ? _x : _y;
            const uint16_t address = _memoryHandler->ReadMem(_programCounter++) + regValue;
            _memoryHandler->WriteMem(address, value);
            cycles += 3;
            break;
        }
        case AddressingMode::Absolute:
        {
            const uint16_t address = _memoryHandler->ReadMem(_programCounter++) + (_memoryHandler->ReadMem(_programCounter++) << 8);
            _memoryHandler->WriteMem(address, value);
            cycles += 3;
            break;
        }
        case AddressingMode::AbsoluteX:
        case AddressingMode::AbsoluteY:
        {
            const uint8_t regValue = (mode == AddressingMode::AbsoluteX) ? _x : _y;
            const uint8_t loByte = _memoryHandler->ReadMem(_programCounter++);
            const uint16_t loBytePlusReg = loByte + regValue;
            const uint8_t hiByte = _memoryHandler->ReadMem(_programCounter++);
            const uint16_t address = loBytePlusReg + (hiByte << 8);
            _memoryHandler->WriteMem(address, value);
            cycles += 4;
            break;
        }
        case AddressingMode::IndirectX:
        {
            const uint16_t address = _memoryHandler->ReadMem(_programCounter++) + _x;
            const uint16_t finalAddress = _memoryHandler->ReadMem(address) + (_memoryHandler->ReadMem(address + 1) << 8);
            _memoryHandler->WriteMem(finalAddress, value);
            cycles += 5;
            break;
        }
        case AddressingMode::IndirectY:
        {
            const uint8_t address = _memoryHandler->ReadMem(_programCounter++);
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
