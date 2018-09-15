#include "IM000_NROM.h"

IM000_NROM::IM000_NROM(Hardware& hw) : MemoryMapper(hw)
{
}

uint8_t IM000_NROM::ReadMem(uint16_t address)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Read);
    return handler.ReadMem(address);
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

size_t IM000_NROM::GetCHRROMMaxSize() const
{
    return kMaxCHRROMSize;
}

uint8_t* IM000_NROM::GetCHRROMPtr()
{
    return _chrROM;
}
