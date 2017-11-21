#include "CPU.h"

int CPU::LDX(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    _x = value;

    SetFlag(Flag::Sign, (_x & (1 << 7)) != 0);
    SetFlag(Flag::Zero, _x == 0);

    return cycles;
}