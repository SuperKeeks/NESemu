#pragma once

#include "MemoryMapper.h"

class IM004_MMC3 : public MemoryMapper
{
public:
    static const size_t kMaxPRGROMSize = 512 * 1024;
    static const size_t kCHRROMBankSize = 1024;
    static const size_t kMaxCHRROMSize = 256 * kCHRROMBankSize;

    IM004_MMC3(Hardware& hw);
    virtual uint8_t ReadCHRROMMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    virtual size_t GetPGRROMMaxSize() const { return kMaxPRGROMSize; }
    virtual uint8_t* GetPGRROMPtr() { return _pgrROM; }
    virtual size_t GetCHRROMMaxSize() const { return kMaxCHRROMSize; }
    virtual uint8_t* GetCHRROMPtr() { return _chrROM; }

private:
    uint8_t _pgrROM[kMaxPRGROMSize];
    uint8_t _chrROM[kMaxCHRROMSize];
    int _registers[8];
    int _bankRegisterToUpdate;
    int _prgROMBankMode;
    int _chrInversion;

    virtual uint8_t ReadPRGROMMem(uint16_t address);
    uint8_t GetCHRROMDataBasedOnRegister(int registerIndex, uint16_t address);
};
