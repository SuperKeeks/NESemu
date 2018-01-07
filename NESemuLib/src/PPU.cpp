#include "PPU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "CHRROM.h"
#include "CPU.h"
#include "LogUtils.h"
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
        if (_currentScanline > kVerticalBlankingScanlinesEnd && !_waitingToShowFrameBuffer)
        {
            _currentScanline = kPreRenderScanline;
        }

        if (_currentScanline == kPreRenderScanline)
        {
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::SpriteOverflow, false);
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::Sprite0Hit, false);
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, false);
        }
        else if (_currentScanline >= kVisibleScanlinesStart && _currentScanline <= kVisibleScanlinesEnd)
        {
            RenderScanline(_currentScanline);
        }
        else if (_currentScanline == kPostRenderScanline)
        {
        }
        else if (_currentScanline == kVerticalBlankingScanlinesStart)
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

        _ticksUntilNextScanline = kCyclesPerScanline - 1;
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
    OMBAssert(!BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpriteSize), "8x16 Sprite size not supported yet!");

    const uint8_t xScroll = _ppuScroll >> 8;
    const uint8_t yScroll = (uint8_t)_ppuScroll;
    const int bkgAbsoluteY = yScroll + index;
    
    FindSpritesInScanline(index);

    for (int i = 0; i < kHorizontalResolution; ++i)
    {
        bool isSprite0;
        SpriteLayer spriteLayer;
        const uint8_t spritePixelColour = CalculateSpriteColourAt(i, index, spriteLayer, isSprite0);
        const uint8_t bkgPixelColour = CalculateBkgColourAt(i, xScroll + i, bkgAbsoluteY);

        // See "Sprite zero hits" section of http://wiki.nesdev.com/w/index.php/PPU_OAM
        if (isSprite0 &&
            i != 255 &&
            spritePixelColour != kTransparentPixelColour &&
            bkgPixelColour != kTransparentPixelColour)
        {
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::Sprite0Hit, true);
        }

        if (spritePixelColour != kTransparentPixelColour &&
            (spriteLayer == SpriteLayer::Front || bkgPixelColour == kTransparentPixelColour))
        {
            RenderPixel(i, index, spritePixelColour);
        }
        else if (bkgPixelColour != kTransparentPixelColour)
        {
            RenderPixel(i, index, bkgPixelColour);
        }
        else
        {
            RenderPixel(i, index, ReadPPUMem(kUniversalBkgColourAddress));
        }
    }
}

void PPU::RenderPixel(int x, int y, uint8_t paletteIndex)
{
    OMBAssert(paletteIndex < sizeofarray(kOutputPalette), "Palette index is out of bounds!");
    const int pixelIndex = y * kHorizontalResolution + x;
    _frameBuffer[pixelIndex] = kOutputPalette[paletteIndex];
}

uint16_t PPU::GetNametableBaseAddress() const
{
    const uint8_t baseNametableValue = _ppuCtrl & 0x3; // Get lower 2 bytes
    switch (baseNametableValue)
    {
        case 0:
            return kNametable0StartAddress;
            break;
        case 1:
            return kNametable1StartAddress;
            break;
        case 2:
            return kNametable2StartAddress;
            break;
        default:
            return kNametable3StartAddress;
            break;
    }
}

uint16_t PPU::GetBkgPatternTableBaseAddress() const
{
    if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::BkgPatternTableAddress))
    {
        return kPatternTable1Address;
    }
    else
    {
        return kPatternTable0Address;
    }
}

uint16_t PPU::GetSpritePatternTableBaseAddress() const
{
    if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpritePatternTableAddress))
    {
        return kPatternTable1Address;
    }
    else
    {
        return kPatternTable0Address;
    }
}

int PPU::GetSpriteHeight() const
{
    return BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpriteSize) ? 16 : 8;
}

void PPU::FindSpritesInScanline(int index)
{
    _secondaryOAMSpriteCount = 0;
    _isSpriteZeroInSecondaryOAM = false;
    
    if (index == 0)
    {
        return; // No sprites allowed in the first scanline
    }

    const int spriteHeight = GetSpriteHeight();

    for (int i = 0; i < kOAMSize && _secondaryOAMSpriteCount < kMaxSpritesInSecondaryOAM; i += kSpriteSize)
    {
        // Add +1 to spriteY because "Sprite data is delayed by one scanline; 
        // you must subtract 1 from the sprite's Y coordinate before writing it here"
        const uint8_t spriteY = _oam[i + kSpriteYPosOffset] + 1;
        if (index >= spriteY && index < spriteY + spriteHeight)
        {
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 0] = _oam[i + 0];
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 1] = _oam[i + 1];
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 2] = _oam[i + 2];
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 3] = _oam[i + 3];
            ++_secondaryOAMSpriteCount;
            if (i == 0)
            {
                _isSpriteZeroInSecondaryOAM = true;
            }
        }
    }
}

uint8_t PPU::CalculateSpriteColourAt(int x, int y, SpriteLayer& layer, bool& isSprite0)
{
    const uint16_t patternTableBaseAddress = GetSpritePatternTableBaseAddress();
    const int spriteHeight = GetSpriteHeight();

    if (BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::SpriteVisibility) &&
        (x >= kLeftClippingPixelCount || BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::SpriteClipping)))
    {
        for (int i = 0; i < _secondaryOAMSpriteCount * kSpriteSize; i += kSpriteSize)
        {
            const uint8_t spriteX = _secondaryOAM[i + kSpriteXPosOffset];
            if (x >= spriteX && x < spriteX + kTileWidth)
            {
                const uint8_t spriteY = _secondaryOAM[i + kSpriteYPosOffset] + 1;
                const uint8_t patternIndex = _secondaryOAM[i + kSpriteTileIndexOffset];
                const uint8_t attributes = _secondaryOAM[i + kSpriteAttributesOffset];
                
                const uint8_t paletteNumber = attributes & 0x3;
                const bool flipHorizontally = (attributes & (1 << 6)) != 0;
                const bool flipVertically = (attributes & (1 << 7)) != 0;
                layer = (attributes & 0x10) == 0 ? SpriteLayer::Front : SpriteLayer::Behind;

                uint8_t patternPixelX;
                if (flipHorizontally)
                {
                    patternPixelX = (kTileWidth - 1) - (x - spriteX);
                }
                else
                {
                    patternPixelX = x - spriteX;
                }

                uint8_t patternPixelY;
                if (flipVertically)
                {
                    patternPixelY = (spriteHeight - 1) - (y - spriteY);
                }
                else
                {
                    patternPixelY = y - spriteY;
                }

                const uint16_t patternBaseAddress = patternTableBaseAddress + (patternIndex << 4) + patternPixelY;
                const bool colourBit0 = (ReadPPUMem(patternBaseAddress) & (0x80 >> patternPixelX)) != 0;
                const bool colourBit1 = (ReadPPUMem(patternBaseAddress + 8) & (0x80 >> patternPixelX)) != 0;
                const uint8_t paletteColour = ((int)colourBit1 << 1) | (int)colourBit0;
                if (paletteColour == 0x0)
                {
                    continue;
                }

                const uint8_t paletteIndex = (1 << 4) | paletteNumber << 2 | paletteColour;
                const uint8_t colourIndex = ReadPPUMem(kPaletteStartAddress + paletteIndex);

                isSprite0 = (i == 0) && _isSpriteZeroInSecondaryOAM;
                return colourIndex;
            }
        }
    }

    // No opaque sprites found at this coordinate
    isSprite0 = false;
    return kTransparentPixelColour; // No opaque sprite pixel found at this position
}

uint8_t PPU::CalculateBkgColourAt(int screenX, int absoluteX, int absoluteY)
{
    if (BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgVisibility) &&
        (screenX >= kLeftClippingPixelCount || BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgClipping)))
    {
        const uint16_t nametableBaseAddress = GetNametableBaseAddress();
        const uint16_t patternTableBaseAddress = GetBkgPatternTableBaseAddress();
        const uint16_t attributeTableBaseAddress = nametableBaseAddress + kAttributeTableStartOffset;

        const uint8_t nametableX = absoluteX / kTileWidth;
        const uint8_t nametableY = absoluteY / kTileHeight;
        const uint16_t nametableIndex = nametableY * kTilesPerNametableRow + nametableX;

        const uint8_t patternPixelX = absoluteX % kTileWidth;
        const uint8_t patternPixelY = absoluteY % kTileHeight;
        const uint8_t patternIndex = ReadPPUMem(nametableBaseAddress + nametableIndex);
        const uint16_t patternBaseAddress = patternTableBaseAddress + (patternIndex << 4) + patternPixelY;
        const bool colourBit0 = (ReadPPUMem(patternBaseAddress) & (0x80 >> patternPixelX)) != 0;
        const bool colourBit1 = (ReadPPUMem(patternBaseAddress + 8) & (0x80 >> patternPixelX)) != 0;
        const uint8_t paletteColour = ((int)colourBit1 << 1) | (int)colourBit0;
        if (paletteColour == 0)
        {
            return kTransparentPixelColour;
        }

        // Each attribute element contains 4x4 tiles, and each subarea is made of 2x2 tiles
        const uint8_t attributeIndex = (nametableY / 4) * 8 + (nametableX / 4);
        const uint8_t attributeArea = (nametableY % 4) / 2 << 1 | ((nametableX % 4) / 2); // 0:Top-Left, 1:Top-Right, 2:Bottom-Left, 3:Bottom-Right
        const uint8_t attributeValue = ReadPPUMem(attributeTableBaseAddress + attributeIndex);
        const uint8_t paletteNumber = (attributeValue >> (attributeArea * 2)) & 0x3;

        const uint8_t paletteIndex = paletteNumber << 2 | paletteColour;
        const uint8_t colourIndex = ReadPPUMem(kPaletteStartAddress + paletteIndex);

        return colourIndex;
    }

    return kTransparentPixelColour;
}
