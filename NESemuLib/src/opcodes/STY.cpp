#include "CPU.h"

int CPU::STY(AddressingMode mode)
{
    int cycles = 1;
    SetValueWithMode(mode, _state._y, cycles);

    return cycles;
}