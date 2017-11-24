#include "CPU.h"

#include "Assert.h"
#include "MemoryHandler.h"

int CPU::JSR(AddressingMode mode)
{
    OMBAssert(mode == AddressingMode::Absolute, "Only supported addressing mode is Absolute");

    uint16_t nextOpAddress = _programCounter + 1;
    Push(GetHighByte(nextOpAddress));
    Push(GetLowByte(nextOpAddress));
    _programCounter = _memoryHandler->ReadMem(++_programCounter) + (_memoryHandler->ReadMem(++_programCounter) << 8);
    return 6;
}
