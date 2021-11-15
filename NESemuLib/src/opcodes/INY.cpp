#include "CPU.h"

int CPU::INY(AddressingMode mode)
{
    ++_state._y;
    SetFlag(Flag::Sign, IsValueNegative(_state._y));
    SetFlag(Flag::Zero, _state._y == 0);
    return 2;
}
