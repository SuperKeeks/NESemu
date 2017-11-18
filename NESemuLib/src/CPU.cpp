#include "CPU.h"

#include "Assert.h"
#include "LogUtils.h"
#include "MemoryHandler.h"

void CPU::PowerOn()
{
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

	if (_opcodes.find(opcode) != _opcodes.end())
	{
		Log::Debug("Executing opcode: $%04X", opcode);
		const int cycles = _opcodes[opcode]();
		Log::Debug("Took %d cycles", cycles);
	}
	else
	{
		Log::Debug("Unknown opcode: $%04X", opcode);
	}

	return 0;
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
		default:
		{
			OMBAssert(false, "Unimplemented AddressingMode!");
		}
	}

	return value;
}
