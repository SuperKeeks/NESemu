#include "IM000_NROM.h"

IM000_NROM::IM000_NROM(Hardware& hw) : MemoryMapper(hw)
{
}

uint8_t IM000_NROM::ReadCHRROMMem(uint16_t address)
{
    return _chrROM[address];
}

void IM000_NROM::WriteMem(uint16_t address, uint8_t value)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Write);
    handler.WriteMem(address, value);
}

uint8_t IM000_NROM::ReadPRGROMMem(uint16_t address)
{
    size_t index;
    if (_pgrROMPageCount == 1)
    {
        index = (address - kPGRROMStartAddress) % 16384;
    }
    else
    {
        index = address - kPGRROMStartAddress;
    }
    return _pgrROM[index];
}
