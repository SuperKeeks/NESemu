#pragma once

#include "AudioChannel.h"

#include "Envelope.h"
#include "LengthCounter.h"
#include "Sweep.h"

class SquareChannel : public AudioChannel
{
public:
    SquareChannel(int channelNumber);

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
    int _channelNumber;
    Envelope _envelope;
    LengthCounter _lengthCounter;
    Sweep _sweep;
    uint8_t _duty;
    int _sequencerStep;
    Divider _timer;
};
