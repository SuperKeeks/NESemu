#pragma once

#include "MemoryMapper.h"

class IM000_NROM : public MemoryMapper
{
public:
    static const size_t kMaxCHRROMSize = 8192;

    IM000_NROM(Hardware& hw);
    virtual uint8_t ReadMem(uint16_t address);
    virtual uint8_t ReadCHRROMMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn() {}
    virtual void Reset() {}

    virtual size_t GetCHRROMMaxSize() const;
    virtual uint8_t* GetCHRROMPtr();

private:
    uint8_t _chrROM[kMaxCHRROMSize];
};