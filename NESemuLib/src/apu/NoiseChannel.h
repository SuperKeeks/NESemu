#pragma once

#include "AudioChannel.h"

class NoiseChannel : public AudioChannel
{
public:
    virtual void SetEnable(bool enable);
    virtual void Tick();
    virtual void QuarterFrameTick();
    virtual void HalfFrameTick();
    virtual int GetOutput() const;

private:
};
