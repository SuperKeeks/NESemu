#include "CPU.h"

int CPU::INX(AddressingMode mode)
{
    ++_state._x;
    SetFlag(Flag::Sign, IsValueNegative(_state._x));
    SetFlag(Flag::Zero, _state._x == 0);
    return 2;
}
