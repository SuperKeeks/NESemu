#include "TriangleChannel.h"

#include "BitwiseUtils.h"
#include "SizeOfArray.h"

void TriangleChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4008)
    {
        const bool lengthCounterHaltLinearCounterControl = BitwiseUtils::GetBitRange(value, 7, 1);
        const int linearCounterLoad = BitwiseUtils::GetBitRange(value, 6, 7);
        _lengthCounter.SetHalt(lengthCounterHaltLinearCounterControl);
        _linearCounter.SetParameters(lengthCounterHaltLinearCounterControl, linearCounterLoad);
    }
    else if (address == 0x400A)
    {
        _period = (_period & 0xFF00) | value; // Set the low 8 bits (keeping the higher bits)
        _timer.SetPeriod(_period + 1);
    }
    else if (address == 0x400B)
    {
        const int highTimerBits = BitwiseUtils::GetBitRange(value, 2, 3);
        _period = (_period & 0xFF) | highTimerBits << 8; // Set the high 3 bits (keeping the low 8 bits)
        _timer.SetPeriod(_period + 1);

        const int lengthCounterLoad = BitwiseUtils::GetBitRange(value, 7, 5);
        _lengthCounter.SetLoad(lengthCounterLoad);

        _linearCounter.SetReloadFlag();
    }
    else if (address != 0x4009)
    {
        OMBAssert(false, "Invalid address!");
    }
}

void TriangleChannel::PowerOn()
{
    Reset();
}

void TriangleChannel::Reset()
{
    _period = 0;
    _sequencerStep = 0;
}

void TriangleChannel::SetEnable(bool enable)
{
    _lengthCounter.SetEnabled(enable);
}

void TriangleChannel::Tick()
{
    if (_timer.Tick() && !_linearCounter.IsZero() && !_lengthCounter.IsZero())
    {
        ++_sequencerStep;
        if (_sequencerStep >= sizeofarray(kSequenceValues))
        {
            _sequencerStep = 0;
        }
    }
}

void TriangleChannel::QuarterFrameTick()
{
    _linearCounter.Tick();
}

void TriangleChannel::HalfFrameTick()
{
    _lengthCounter.Tick();
}

int TriangleChannel::GetOutput() const
{
    return kSequenceValues[_sequencerStep];
}

bool TriangleChannel::IsLengthCounterZero() const
{
    return _lengthCounter.IsZero();
}
