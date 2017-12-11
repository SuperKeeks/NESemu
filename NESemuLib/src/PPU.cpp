#include "PPU.h"

#include "Assert.h"
#include "CHRROM.h"
#include "SizeOfArray.h"

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
        const uint16_t ppuAddress = ConvertToRealVRAMAddress(_ppuAddr);
        _ppuAddr += GetAddressIncrement();
        return ReadPPUMem(ppuAddress);
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
        const uint16_t ppuAddress = ConvertToRealVRAMAddress(_ppuAddr);
        _ppuAddr += GetAddressIncrement();
        WritePPUMem(ppuAddress, value);
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
    _ppuCtrl = 0;
    _ppuMask = 0;
    _ppuStatus = 0;
    _oamAddr = 0;
    _ppuScroll = 0;
    _ppuAddr = 0;

    for (int i = 0; i < sizeofarray(_vram); ++i)
    {
        _vram[i] = 0;
    }

    for (int i = 0; i < sizeofarray(_oam); ++i)
    {
        _oam[i] = 0;
    }
}

void PPU::Reset(CHRROM* chrRom, MirroringMode mirroringMode)
{
    _chrRom = chrRom;
    _mirroringMode = mirroringMode;
}

uint16_t PPU::ConvertToRealVRAMAddress(uint16_t address) const
{
    return address % kShadowVRAMStartAddress; // Shadow VRAM
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

uint8_t* PPU::GetNametableMem(uint16_t address)
{
    if (address >= kNametable0StartAddress && address < kNametable1StartAddress)
    {
        return &_vram[address - kNametable0StartAddress];
    }
    else if (address >= kNametable1StartAddress && address < kNametable2StartAddress)
    {
        const int offset = address % kNametable1StartAddress;
        if (_mirroringMode == MirroringMode::Vertical)
        {
            return &_vram[kNametableSize + offset];
        }
        else
        {
            return &_vram[offset];
        }
    }
    else if (address >= kNametable2StartAddress && address < kNametable3StartAddress)
    {
        const int offset = address % kNametable2StartAddress;
        if (_mirroringMode == MirroringMode::Vertical)
        {
            return &_vram[offset];
        }
        else
        {
            return &_vram[kNametableSize + offset];
        }
    }
    else if (address >= kNametable3StartAddress && address < kMirrorStartAddress)
    {
        const int offset = address % kNametable3StartAddress;
        return &_vram[kNametableSize + offset];
    }
    else
    {
        OMBAssert(false, "$%04X address is not a nametable address", address);
        return 0;
    }
}

uint8_t PPU::ReadPPUMem(uint16_t address)
{
    if (address < kNametable0StartAddress)
    {
        return _chrRom->ReadMem(address);
    }
    else if (address < kMirrorStartAddress)
    {
        return *GetNametableMem(address);
    }
    else if (address < kPaletteStartAddress)
    {
        return *GetNametableMem(address - 0x1000);
    }
    else if (address < 0x3F20)
    {
        OMBAssert(false, "TODO Palette");
        return 0;
    }
    else if (address < 0x4000)
    {
        OMBAssert(false, "TODO Shadow Palette");
        return 0;
    }
    else
    {
        OMBAssert(false, "Unimplemented");
        return 0;
    }
}

void PPU::WritePPUMem(uint16_t address, uint8_t value)
{
    if (address >= kNametable0StartAddress && address < kMirrorStartAddress)
    {
        *GetNametableMem(address) = value;
    }
    else if (address >= kMirrorStartAddress && address < kPaletteStartAddress)
    {
        *GetNametableMem(address - 0x1000) = value;
    }
    else
    {
        OMBAssert(false, "PPU memory at $%04X is not writtable", address);
    }
}
