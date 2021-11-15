#include "CPU.h"

int CPU::LDX(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    _state._x = value;

    SetFlag(Flag::Sign, IsValueNegative(_state._x));
    SetFlag(Flag::Zero, _state._x == 0);

    return cycles;
}