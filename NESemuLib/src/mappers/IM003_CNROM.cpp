#include "IM003_CNROM.h"

IM003_CNROM::IM003_CNROM(Hardware& hw) : MemoryMapper(hw)
{
}

uint8_t IM003_CNROM::ReadMem(uint16_t address)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Read);
    return handler.ReadMem(address);
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

size_t IM003_CNROM::GetCHRROMMaxSize() const
{
    return kMaxCHRROMSize;
}

uint8_t* IM003_CNROM::GetCHRROMPtr()
{
    return _chrROM;
}
