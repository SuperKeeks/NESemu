#include "CPU.h"

#include "MemoryMapper.h"

int CPU::BRK(AddressingMode mode)
{
    ++_programCounter;
    Interrupt(true, kInterruptBreakVectorAddressL);

    return 7;
}