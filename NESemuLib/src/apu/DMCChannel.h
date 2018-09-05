#pragma once

#include "AudioChannel.h"

#include "Divider.h"

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
    Divider _timer;
    int _outputLevel;
    int _remainingBits;
    bool _silenceFlag;
    int _rightShiftRegister;
    int _sampleBuffer;
};
