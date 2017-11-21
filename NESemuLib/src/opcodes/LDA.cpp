#include "CPU.h"

int CPU::LDA(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    _accumulator = value;

    SetFlag(Flag::Sign, IsValueNegative(_accumulator));
    SetFlag(Flag::Zero, _accumulator == 0);

    return cycles;
}