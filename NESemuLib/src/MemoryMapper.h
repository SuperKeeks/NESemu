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
    
    static const uint16_t kPRGROMPageSize = 16384;
    static const uint16_t kPGRROMStartAddress = 0x8000;
    static const size_t kCHRROMPageSize = 8192;
    static const size_t kUnitTestPGRROMSize = 2 * kPRGROMPageSize;

    MemoryMapper(Hardware& hw, size_t pgrPageCount, size_t chrPageCount);
    virtual ~MemoryMapper() {}
    virtual uint8_t ReadMem(uint16_t address);
    virtual uint8_t ReadCHRROMMem(uint16_t address) = 0;
    virtual void WriteMem(uint16_t address, uint8_t value) {}
    virtual void OnVisibleScanlineEnd() {}
    virtual void PowerOn() = 0;
    virtual void Reset() = 0;

    // For ROM parsing only
    virtual size_t GetPGRROMMaxSize() const = 0;
    virtual uint8_t* GetPGRROMPtr() = 0;
    virtual size_t GetCHRROMMaxSize() const = 0;
    virtual uint8_t* GetCHRROMPtr() = 0;

protected:
    Hardware& _hw;
    size_t _pgrROMPageCount;
    size_t _chrPageCount;

    MemoryHandler& GetMemoryHandlerForAddress(uint16_t address, AccessMode mode);
    virtual uint8_t ReadPRGROMMem(uint16_t address) = 0;
};
