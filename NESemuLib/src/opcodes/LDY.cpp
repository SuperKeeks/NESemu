#include "CPU.h"

int CPU::LDY(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    _state._y = value;

    SetFlag(Flag::Sign, IsValueNegative(_state._y));
    SetFlag(Flag::Zero, _state._y == 0);

    return cycles;
}