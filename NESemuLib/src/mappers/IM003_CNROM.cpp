#include "IM003_CNROM.h"

IM003_CNROM::IM003_CNROM(Hardware& hw) : MemoryMapper(hw)
{
}

uint8_t IM003_CNROM::ReadCHRROMMem(uint16_t address)
{
    return _chrROM[(_chrROMPageIndex * kPageCHRROMSize) + address];
}

void IM003_CNROM::WriteMem(uint16_t address, uint8_t value)
{
    if (address >= 0x8000 && address <= 0xFFFF)
    {
        _chrROMPageIndex = value & 0x3;
    }
    else
    {
        MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Write);
        handler.WriteMem(address, value);
    }
}

void IM003_CNROM::Reset()
{
    _chrROMPageIndex = 0;
}

uint8_t IM003_CNROM::ReadPRGROMMem(uint16_t address)
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
