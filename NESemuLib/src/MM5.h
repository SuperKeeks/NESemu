#pragma once

#include "MemoryHandler.h"

class MM5 : public MemoryHandler
{
public:
    MM5();
    virtual ~MM5();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();
};
