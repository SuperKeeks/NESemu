#pragma once

#include "MemoryMapper.h"

class IM003_CNROM : public MemoryMapper
{
public:
    static const size_t kPageCHRROMSize = 8192;
    static const size_t kMaxPageCount = 4;
    static const size_t kMaxCHRROMSize = kMaxPageCount * kPageCHRROMSize;

    IM003_CNROM(Hardware& hw);
    virtual uint8_t ReadMem(uint16_t address);
    virtual uint8_t ReadCHRROMMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn() {}
    virtual void Reset();

    // For CHRROM parsing only
    virtual size_t GetCHRROMMaxSize() const;
    virtual uint8_t* GetCHRROMPtr();

private:
    uint8_t _chrROM[kMaxCHRROMSize];
    int _chrROMPageIndex;
};
