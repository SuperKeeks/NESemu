#pragma once

#include "MemoryHandler.h"

class AudioChannel : public MemoryHandler
{
public:
    virtual uint8_t ReadMem(uint16_t address) { OMBAssert(false, "Can't read audio channels!"); return 0; }

    virtual void Tick() {}
    virtual void QuarterFrameTick() {}
    virtual void HalfFrameTick() {}
};
