#pragma once

#include "MemoryMapper.h"

class IM004_MMC3 : public MemoryMapper
{
public:
    IM004_MMC3(Hardware& hw);
    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    // For CHRROM parsing only
    //uint8_t* GetCHRROMPtr() { return _chrRom; }

private:
    int _registers[8];
    int _bankRegisterToUpdate;
    int _prgROMBankMode;
    int _chrInversion;
};
