#pragma once

#include "MemoryHandler.h"
#include "MirroringMode.h"

class CHRROM;

class PPU : public MemoryHandler
{
public:
    static const int kVRAMSize = 2048;
    static const int kSpriteSize = 4;
    static const int kSpriteCount = 64;
    static const int kNametable0StartAddress = 0x2000;
    static const int kNametable1StartAddress = 0x2400;
    static const int kNametable2StartAddress = 0x2800;
    static const int kNametable3StartAddress = 0x2C00;
    static const int kNametableSize = 0x400;
    static const int kMirrorStartAddress = 0x3000;
    static const int kPaletteStartAddress = 0x3F00;
    static const int kShadowVRAMStartAddress = 0x4000;

    PPU();
    virtual ~PPU();

    // Important: These refer to the memory as seen by the rest of the system (i.e. CPU)
    // This is different from the internally-addressed space
    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);

    virtual void PowerOn();
    virtual void Reset(CHRROM* chrRom, MirroringMode mirroringMode);

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

    CHRROM* _chrRom;
    MirroringMode _mirroringMode;

    uint8_t* GetNametableMem(uint16_t address);
    uint8_t ReadPPUMem(uint16_t address);
    void WritePPUMem(uint16_t address, uint8_t value);
    uint16_t ConvertToRealVRAMAddress(uint16_t address) const;
    uint8_t GetAddressIncrement() const;
};
