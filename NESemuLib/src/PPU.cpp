#include "PPU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "CHRROM.h"
#include "CPU.h"
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
        // Reset PPU Scroll and Addr write toggle so next write to any of them sets the high byte
        _writeToggle = false;

        // Reset VBlank flag
        const uint8_t statusBeforeVBlankReset = _ppuStatus;
        if (_ppuStatus != 0)
        {
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, false);
        }

        return statusBeforeVBlankReset;
    }
    else if (address == 0x2004)
    {
        return _oam[_oamAddr];
    }
    else if (address == 0x2007)
    {
        const uint16_t ppuAddress = ConvertToRealVRAMAddress(_ppuAddr);
        _ppuAddr += GetAddressIncrement();
        const uint8_t prevReadBuffer = _readBuffer;
        _readBuffer = ReadPPUMem(ppuAddress);

        if (ppuAddress < kPaletteStartAddress)
        {
            // Non-palette values are buffered, so it requires 2 reads after address set (via 0x2006) to get the value you expect
            return prevReadBuffer;
        }
        else
        {
            // Palette values are returned immediately (although also cached)
            return _readBuffer;
        }
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
        const bool wasNMIFlagSet = BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::ExecuteNMIOnVBlank);
        _ppuCtrl = value;

        // "When turning on the NMI flag in bit 7, if the PPU is currently in vertical blank 
        // and the PPUSTATUS ($2002) vblank flag is set, an NMI will be generated immediately"
        if (!wasNMIFlagSet && BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::ExecuteNMIOnVBlank) && BitwiseUtils::IsFlagSet(_ppuStatus, PPUStatusFlags::VBlank))
        {
            _cpu->ExecuteNMI();
        }
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
        _oam[_oamAddr++] = value;
    }
    else if (address == 0x2005)
    {
        WriteToggleableRegister(_ppuScroll, value);
    }
    else if (address == 0x2006)
    {
        WriteToggleableRegister(_ppuAddr, value);
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
    else if (address == kDMARegisterAddress)
    {
        for (int i = 0; i < sizeofarray(_oam); ++i)
        {
            _oam[i] = _memoryHandler->ReadMem((value << 8) + i);
        }
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

void PPU::Reset(MemoryHandler* memoryHandler, CPU* cpu, CHRROM* chrRom, MirroringMode mirroringMode)
{
    _memoryHandler = memoryHandler;
    _cpu = cpu;
    _chrRom = chrRom;
    _mirroringMode = mirroringMode;
}

void PPU::Tick()
{
    // Main reference for this function implementation: http://wiki.nesdev.com/w/index.php/PPU_rendering
    --_ticksUntilNextScanline;
    if (_ticksUntilNextScanline <= 0)
    {
        ++_currentScanline;
        if (_currentScanline > 260 && !_waitingToShowFrameBuffer)
        {
            _currentScanline = -1;
        }

        if (_currentScanline == -1)
        {
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, false);
        }
        else if (_currentScanline >= 0 && _currentScanline <= 239)
        {
            RenderScanline(_currentScanline);
        }
        else if (_currentScanline == 240)
        {
        }
        else if (_currentScanline == 241)
        {
            if (_waitToShowFrameBuffer)
            {
                _waitingToShowFrameBuffer = true;
            }
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, true);

            // Execute NMI on VBlank
            if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::ExecuteNMIOnVBlank))
            {
                _cpu->ExecuteNMI();
            }
        }
        else
        {
        }

        _ticksUntilNextScanline = 341 - 1;
    }
}

uint32_t* PPU::GetFrameBuffer()
{
    _waitingToShowFrameBuffer = false;
    return _frameBuffer;
}

void PPU::SetMirroringMode(MirroringMode mirroringMode)
{
    _mirroringMode = mirroringMode;
}

uint16_t PPU::ConvertToRealVRAMAddress(uint16_t address) const
{
    return address % kShadowVRAMStartAddress; // Shadow VRAM
}

uint8_t PPU::GetAddressIncrement() const
{
    if (!BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::AddressIncrement))
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
    else if (address < 0x4000)
    {
        const uint8_t index = ConvertAddressToPaletteIndex(address);
        return _palettes[index];
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
    else if (address >= kPaletteStartAddress && address < 0x4000)
    {
        const uint8_t index = ConvertAddressToPaletteIndex(address);
        _palettes[index] = value;
    }
    else
    {
        OMBAssert(false, "PPU memory at $%04X is not writtable", address);
    }
}

uint8_t PPU::ConvertAddressToPaletteIndex(uint16_t address) const
{
    const uint16_t realAddress = ((address - kPaletteStartAddress) % 0x20) + kPaletteStartAddress;
    int index = -1;
    if (realAddress == 0x3F00 || realAddress == 0x3F10)
    {
        index = 0;
    }
    else if (realAddress >= 0x3F01 && realAddress <= 0x3F0F)
    {
        // Background palettes 0-3 + their extra data
        index = realAddress - kPaletteStartAddress;
    }
    else if (realAddress == 0x3F10 || realAddress == 0x3F14 || realAddress == 0x3F18 || realAddress == 0x3F1C)
    {
        index = realAddress - 0x3F10; // Mirrors
    }
    else if (realAddress >= 0x3F11 && realAddress <= 0x3F13)
    {
        // Sprite palette 0
        index = realAddress - 0x3F11 + 16;
    }
    else if (realAddress >= 0x3F15 && realAddress <= 0x3F17)
    {
        // Sprite palette 1
        index = realAddress - 0x3F11 - 1 + 16;
    }
    else if (realAddress >= 0x3F19 && realAddress <= 0x3F1B)
    {
        // Sprite palette 2
        index = realAddress - 0x3F11 - 2 + 16;
    }
    else if (realAddress >= 0x3F1D && realAddress <= 0x3F1F)
    {
        // Sprite palette 3
        index = realAddress - 0x3F11 - 3 + 16;
    }
    else
    {
        OMBAssert(false, "Address out of palette space");
    }

    OMBAssert(index >= 0 && index <= sizeofarray(_palettes), "Index out of bounds!");
    return index;
}

void PPU::WriteToggleableRegister(uint16_t& reg, uint8_t value)
{
    if (_writeToggle)
    {
        reg = value + (reg & 0xFF00);
    }
    else
    {
        reg = (value << 8) + (reg & 0xFF);
    }
    _writeToggle = !_writeToggle;
}

void PPU::RenderScanline(int index)
{
    OMBAssert(index >= 0 && index < kHorizontalResolution, "Scanline index out of bounds!");

    if (BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgVisibility))
    {
        const uint8_t bkgPaletteIndex = ReadPPUMem(kBkgColourAddress);
        
        if (BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgClipping))
        {
            for (int i = 0; i < kLeftClippingPixelCount; ++i)
            {
                RenderPixel(i, index, bkgPaletteIndex);
            }
        }

        for (int i = kLeftClippingPixelCount; i < kHorizontalResolution; ++i)
        {
            RenderPixel(i, index, bkgPaletteIndex);
        }
    }
}

void PPU::RenderPixel(int x, int y, uint8_t paletteIndex)
{
    OMBAssert(paletteIndex < sizeofarray(kOutputPalette), "Palette index is out of bounds!");
    const int pixelIndex = y * kHorizontalResolution + x;
    _frameBuffer[pixelIndex] = kOutputPalette[paletteIndex];
}
