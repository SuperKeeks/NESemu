#include "PPU.h"

#include "Assert.h"

PPU::PPU()
{
}

PPU::~PPU()
{
}

uint8_t PPU::ReadMem(uint16_t address)
{
    if (address == 0x2002)
    {
        return _ppuStatus;
    }
    else if (address == 0x2004)
    {
        _oamAddr++;
        OMBAssert(false, "TODO");
        return 0;
    }
    else if (address == 0x2007)
    {
        const uint16_t address = ConvertToRealVRAMAddress(_ppuAddr);
        _ppuAddr += GetAddressIncrement();
        return _vram[address];
    }
    else if (address >= 0x2008 && address < 0x4000)
    {
        // Shadow PPU Registers
        return ReadMem(((address - 0x2000) % (0x2008 - 0x2000)) + 0x2000);
    }
    else
    {
        OMBAssert(false, "Unimplemented");
        return 0;
    }
}

void PPU::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x2000)
    {
        _ppuCtrl = value;
    }
    else if (address == 0x2001)
    {
        _ppuMask = value;
    }
    else if (address == 0x2003)
    {
        _oamAddr = value;
    }
    else if (address == 0x2004)
    {
        _oamAddr++;
        OMBAssert(false, "TODO");
    }
    else if (address == 0x2005)
    {
        OMBAssert(false, "TODO");
    }
    else if (address == 0x2006)
    {
        OMBAssert(false, "TODO");
    }
    else if (address == 0x2007)
    {
        const uint16_t address = ConvertToRealVRAMAddress(_ppuAddr);
        _ppuAddr += GetAddressIncrement();
        _vram[address] = value;
    }
    else if (address >= 0x2008 && address < 0x4000)
    {
        // Shadow PPU Registers
        WriteMem(((address - 0x2000) % (0x2008 - 0x2000)) + 0x2000, value);
    }
    else
    {
        OMBAssert(false, "Unimplemented");
    }
}

void PPU::PowerOn()
{
    // TODO
}

void PPU::Reset()
{
    // TODO
}

uint16_t PPU::ConvertToRealVRAMAddress(uint16_t address) const
{
    if (address >= 0x3F10 && address < 0x4000)
    {
        return ((address - 0x3F00) % (0x3F20 - 0x3F00)) + 0x3F00; // Shadow palette
    }
    else
    {
        return address % kVRAMSize; // Shadow VRAM
    }
}

uint8_t PPU::GetAddressIncrement() const
{
    if ((_ppuCtrl & (1 << 2)) == 0)
    {
        return 1;
    }
    else
    {
        return 32;
    }
}
