#include "CPU.h"

#include "MemoryMapper.h"

int CPU::BRK(AddressingMode mode)
{
    ++_state._programCounter;
    Interrupt(true, kInterruptBreakVectorAddressL);

    return 7;
}