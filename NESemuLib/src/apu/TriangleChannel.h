#pragma once

#include "AudioChannel.h"

#include "Divider.h"
#include "LengthCounter.h"
#include "LinearCounter.h"

class TriangleChannel : public AudioChannel
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
    const int kSequenceValues[32] = {
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };

    Divider _timer;
    LengthCounter _lengthCounter;
    LinearCounter _linearCounter;
    int _period = 0;
    int _sequencerStep = 0;
};
