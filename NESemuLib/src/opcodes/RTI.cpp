#include "CPU.h"

int CPU::RTI(AddressingMode mode)
{
    _state._status = Pop() & ~(1 << Flag::Break) & ~(1 << Flag::Unused);
    _state._programCounter = Pop() | (Pop() << 8);

    return 6;
}