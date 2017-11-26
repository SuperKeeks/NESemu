#include "CPU.h"

int CPU::CMP(AddressingMode mode, uint8_t registerValue)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    SetFlag(CPU::Flag::Sign, registerValue >= 0x80);
    SetFlag(CPU::Flag::Zero, registerValue == value);
    SetFlag(CPU::Flag::Carry, registerValue >= value);

    return cycles;
}
