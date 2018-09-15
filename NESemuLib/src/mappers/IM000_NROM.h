#pragma once

#include "MemoryMapper.h"

class IM000_NROM : public MemoryMapper
{
public:
    static const uint16_t kMaxPRGROMSize = 2 * kPRGROMPageSize;
    static const size_t kMaxCHRROMSize = 8192;

    IM000_NROM(Hardware& hw);
    virtual uint8_t ReadCHRROMMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn() {}
    virtual void Reset() {}

    virtual size_t GetPGRROMMaxSize() const { return kMaxPRGROMSize; }
    virtual uint8_t* GetPGRROMPtr() { return _pgrROM; }
    virtual size_t GetCHRROMMaxSize() const { return kMaxCHRROMSize; }
    virtual uint8_t* GetCHRROMPtr() { return _chrROM; }

private:
    uint8_t _pgrROM[kMaxPRGROMSize];
    uint8_t _chrROM[kMaxCHRROMSize];

    virtual uint8_t ReadPRGROMMem(uint16_t address);
};