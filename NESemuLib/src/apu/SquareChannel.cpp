#include "SquareChannel.h"

#include "BitwiseUtils.h"
#include "LogUtils.h"

void SquareChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4000 || address == 0x4004)
    {
        const uint8_t volume = BitwiseUtils::GetBitRange(value, 3, 4);
        const bool constantVolumeFlag = BitwiseUtils::GetBitRange(value, 4, 1) == 1;
        const bool envelopeLoopFlag = BitwiseUtils::GetBitRange(value, 5, 1) == 1;
        _duty = BitwiseUtils::GetBitRange(value, 7, 2);
        _envelope.SetParameters(envelopeLoopFlag, constantVolumeFlag, volume);
    }
    else if (address == 0x4001 || address == 0x4005)
    {
        // TODO
    }
    else if (address == 0x4002 || address == 0x4006)
    {
        int period = _timer.GetPeriod();
        period = (period & 0xFF00) | value; // Set the low 8 bits (keeping the higher bits)
        _timer.SetPeriod(period);
    }
    else if (address == 0x4003 || address == 0x4007)
    {
        int highTimerBits = BitwiseUtils::GetBitRange(value, 2, 3);
        int period = _timer.GetPeriod();
        period = (period & 0xFF) | highTimerBits << 7; // Set the high 3 bits (keeping the low 8 bits)
        _timer.SetPeriod(period);

        // TODO: 
        // The sequencer is immediately restarted at the first value of the current sequence. 
        // (DONE)The envelope is also restarted. 
        // The period divider is not reset.
        _envelope.SetStartFlag();
    }
    else
    {
        OMBAssert(false, "Unhandled address!");
    }
}

void SquareChannel::PowerOn()
{
    Reset();
}

void SquareChannel::Reset()
{
    _duty = 0;
    _sequencerStep = 0;
}

void SquareChannel::Tick()
{
    if (_timer.Tick())
    {
        ++_sequencerStep;
        if (_sequencerStep >= 8)
        {
            _sequencerStep = 0;
        }
    }
}

void SquareChannel::QuarterFrameTick()
{
    _envelope.Tick();
    // TODO: Linear counter Tick()
}

void SquareChannel::HalfFrameTick()
{
    // TODO: Length counter Tick() and sweep Tick()    
}

int SquareChannel::GetOutput() const
{
    const int envelopeOutput = _envelope.GetOutput();

    // TODO: Sweep and length counter
    int output = envelopeOutput * dutyValues[_duty][_sequencerStep];
    return output;
}
