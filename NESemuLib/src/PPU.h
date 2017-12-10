#pragma once

#include "MemoryHandler.h"

class PPU : public MemoryHandler
{
public:
    static const int kVRAMSize = 2048;
    static const int kSpriteSize = 4;
    static const int kSpriteCount = 64;

    PPU();
    virtual ~PPU();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

private:
    // Registers
    uint8_t _ppuCtrl;
    uint8_t _ppuMask;
    uint8_t _ppuStatus;
    uint8_t _oamAddr;
    uint16_t _ppuScroll;
    uint16_t _ppuAddr;

    uint8_t _vram[kVRAMSize];
    uint8_t _oam[kSpriteSize * kSpriteCount];

    uint16_t ConvertToRealVRAMAddress(uint16_t address) const;
    uint8_t GetAddressIncrement() const;
};
