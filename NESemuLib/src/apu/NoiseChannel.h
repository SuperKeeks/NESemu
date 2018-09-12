#pragma once

#include "AudioChannel.h"

#include "Divider.h"
#include "Envelope.h"
#include "LengthCounter.h"

class NoiseChannel : public AudioChannel
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

    bool IsLengthCounterZero() const;

private:
    Envelope _envelope;
    Divider _timer;
    LengthCounter _lengthCounter;
    bool _modeFlag = false;
    int _shiftRegister = 0;
};
