#include "MemoryMapper.h"

MemoryMapper::MemoryMapper(Hardware& hw, size_t pgrPageCount, size_t chrPageCount) : 
    _hw(hw),
    _pgrROMPageCount(pgrPageCount),
    _chrPageCount(chrPageCount)
{
}

uint8_t MemoryMapper::ReadMem(uint16_t address)
{
    if (address >= 0x8000)
    {
        return ReadPRGROMMem(address);
    }
    else
    {
        MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Read);
        return handler.ReadMem(address);
    }
}

MemoryHandler& MemoryMapper::GetMemoryHandlerForAddress(uint16_t address, AccessMode mode)
{
    if (address >= 0x0 && address < 0x2000)
    {
        return _hw.ram;
    }
    else if ((address >= 0x2000 && address < 0x4000) || address == 0x4014)
    {
        return _hw.ppu;
    }
    else if ((address >= 0x4000 && address < 0x4014) || address == 0x4015 || (address == 0x4017 && mode == AccessMode::Write))
    {
        return _hw.apu;
    }
    else if (address == 0x4016 || (address == 0x4017 && mode == AccessMode::Read))
    {
        return _hw.input;
    }
    else if (address >= 0x6000 && address < 0x8000)
    {
        return _hw.sram;
    }
    else
    {
        OMBAssert(false, "Unimplemented!");
        return *(new MemoryHandler());
    }
}
