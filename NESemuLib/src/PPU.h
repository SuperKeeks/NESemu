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
    static const int kOAMSize = kSpriteCount * kSpriteSize;
    static const int kPaletteArraySize = 1 + 4*3 + 1*3 + 4*3; // 1 for Bkg color + 4 Background palettes with extra data between them + 4 Sprite palettes
    static const int kNametable0StartAddress = 0x2000;
    static const int kNametable1StartAddress = 0x2400;
    static const int kNametable2StartAddress = 0x2800;
    static const int kNametable3StartAddress = 0x2C00;
    static const int kNametableSize = 0x400;
    static const int kMirrorStartAddress = 0x3000;
    static const int kPaletteStartAddress = 0x3F00;
    static const int kShadowVRAMStartAddress = 0x4000;
    static const int kDMARegisterAddress = 0x4014;
    static const int kPPUStatusAddress = 0x2002;
    static const int kOAMAddress = 0x2003;
    static const int kOAMData = 0x2004;
    static const int kPPUAddrAddress = 0x2006;
    static const int kPPUDataAddress = 0x2007;

    PPU();
    virtual ~PPU();

    // Important: These refer to the memory as seen by the rest of the system (i.e. CPU)
    // This is different from the internally-addressed space
    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);

    virtual void PowerOn();
    virtual void Reset(MemoryHandler* memoryHandler, CHRROM* chrRom, MirroringMode mirroringMode);

    void Tick();

    // For testing purposes
    void SetMirroringMode(MirroringMode mirroringMode);

private:
    // Registers
    uint8_t _ppuCtrl;
    uint8_t _ppuMask;
    uint8_t _ppuStatus;
    uint8_t _oamAddr;
    uint16_t _ppuScroll;
    uint16_t _ppuAddr;

    // Status variables legend: 1: next write to 0x2005 sets upper byte, 0: next write sets lower byte, -1: needs reset (through 0x2002 read)
    int _ppuScrollStatus;
    int _ppuAddrStatus;

    uint8_t _readBuffer;

    uint8_t _vram[kVRAMSize];
    uint8_t _oam[kSpriteSize * kSpriteCount];
    uint8_t _palettes[kPaletteArraySize];

    MemoryHandler* _memoryHandler;
    CHRROM* _chrRom;
    MirroringMode _mirroringMode;

    uint8_t* GetNametableMem(uint16_t address);
    uint8_t ReadPPUMem(uint16_t address);
    void WritePPUMem(uint16_t address, uint8_t value);
    uint16_t ConvertToRealVRAMAddress(uint16_t address) const;
    uint8_t GetAddressIncrement() const;
    uint8_t ConvertAddressToPaletteIndex(uint16_t address) const;
};
