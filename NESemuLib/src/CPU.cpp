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

	Log::Debug("Next opcode is: %d", opcode);

	return 0;
}
