#pragma once

#include "MemoryHandler.h"

class AudioChannel : public MemoryHandler
{
public:
    virtual uint8_t ReadMem(uint16_t address) { OMBAssert(false, "Can't read audio channels!"); return 0; }

    virtual void SetEnable(bool enable) = 0;
    virtual void Tick() = 0;
    virtual void QuarterFrameTick() = 0;
    virtual void HalfFrameTick() = 0;
    virtual int GetOutput() const = 0;
};
