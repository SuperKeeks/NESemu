#pragma once

#include "AudioChannel.h"

class DMCChannel : public AudioChannel
{
public:
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    virtual void SetEnable(bool enable);
    virtual void Tick();
    virtual void QuarterFrameTick();
    virtual void HalfFrameTick();
    virtual int GetOutput() const;

private:
    int _outputLevel;
};
