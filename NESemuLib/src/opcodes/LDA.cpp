#include "CPU.h"

#include "LogUtils.h"

int CPU::LDA(AddressingMode mode)
{
	int cycles = 1;
	const uint8_t value = GetValueWithMode(mode, cycles);
	_accumulator = value;

	return cycles;
}