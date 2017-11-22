#include "CPU.h"

int CPU::INX(AddressingMode mode)
{
    ++_x;
    SetFlag(Flag::Sign, IsValueNegative(_x) != 0);
    SetFlag(Flag::Zero, _x == 0);
    return 2;
}
