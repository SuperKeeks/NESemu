#include "CPU.h"

int CPU::LDA(AddressingMode mode)
{
	int cycles = 1;
	const uint8_t value = GetValueWithMode(mode, cycles);
	_accumulator = value;

	SetFlag(Flag::Sign, (_accumulator & (1 << 7)) != 0);
	SetFlag(Flag::Zero, _accumulator == 0);

	return cycles;
}