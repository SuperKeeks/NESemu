#include "CPU.h"

#include "Assert.h"
#include "MemoryHandler.h"

int CPU::JMP(AddressingMode mode)
{
    switch (mode)
    {
        case AddressingMode::Absolute:
        {
            _programCounter = _memoryHandler->ReadMem(_programCounter++) + (_memoryHandler->ReadMem(_programCounter++) << 8);
            return 3;
        }
        case AddressingMode::Indirect:
        {
            const uint8_t addressLo = _memoryHandler->ReadMem(_programCounter++);
            const uint8_t addressHi = _memoryHandler->ReadMem(_programCounter++);
            const uint16_t finalAddress1 = addressLo + (addressHi << 8);
            const uint8_t addressLoPlus1 = addressLo + 1;
            const uint16_t finalAddress2 = addressLoPlus1 + (addressHi << 8);
            _programCounter = _memoryHandler->ReadMem(finalAddress1) + (_memoryHandler->ReadMem(finalAddress2) << 8);

            return 5;
        }
        default:
        {
            OMBAssert(false, "Unsupported addressing mode!");
            return 0;
        }
    }
}
