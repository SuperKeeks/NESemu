#pragma once

#include "Assert.h"
#include "Hardware.h"

#include <stdint.h>

class MemoryMapper
{
public:
    enum AccessMode
    {
        Read,
        Write
    };
    
    static const size_t kCHRROMPageSize = 8192;

    MemoryMapper(Hardware& hw);
    virtual ~MemoryMapper() {}
    virtual uint8_t ReadMem(uint16_t address) = 0;
    virtual uint8_t ReadCHRROMMem(uint16_t address) = 0;
    virtual void WriteMem(uint16_t address, uint8_t value) {}
    virtual void PowerOn() = 0;
    virtual void Reset() = 0;

    // For CHRROM parsing only
    virtual size_t GetCHRROMMaxSize() const = 0;
    virtual uint8_t* GetCHRROMPtr() = 0;

protected:
    Hardware& _hw;

    MemoryHandler& GetMemoryHandlerForAddress(uint16_t address, AccessMode mode);
};
