#include "SquareChannel.h"

#include "BitwiseUtils.h"
#include "LogUtils.h"

SquareChannel::SquareChannel(int channelNumber) : _channelNumber(channelNumber)
{
}

void SquareChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4000 || address == 0x4004)
    {
        const uint8_t volume = BitwiseUtils::GetBitRange(value, 3, 4);
        const bool constantVolumeFlag = BitwiseUtils::GetBitRange(value, 4, 1) == 1;
        const bool envelopeLoopFlag = BitwiseUtils::GetBitRange(value, 5, 1) == 1;
        _duty = BitwiseUtils::GetBitRange(value, 7, 2);
        _envelope.SetParameters(envelopeLoopFlag, constantVolumeFlag, volume);
        _lengthCounter.SetHalt(envelopeLoopFlag);
    }
    else if (address == 0x4001 || address == 0x4005)
    {
        const bool enabled = BitwiseUtils::GetBitRange(value, 7, 1) == 1;
        const int sweepPeriod = BitwiseUtils::GetBitRange(value, 6, 3);
        const bool negateFlag = BitwiseUtils::GetBitRange(value, 3, 1) == 1;
        const int shiftCount = BitwiseUtils::GetBitRange(value, 2, 3);
        _sweep.SetParameters(_channelNumber, enabled, sweepPeriod, negateFlag, shiftCount);
    }
    else if (address == 0x4002 || address == 0x4006)
    {
        int period = _timer.GetPeriod();
        period = (period & 0xFF00) | value; // Set the low 8 bits (keeping the higher bits)
        _timer.SetPeriod(period);
    }
    else if (address == 0x4003 || address == 0x4007)
    {
        const int highTimerBits = BitwiseUtils::GetBitRange(value, 2, 3);
        int period = _timer.GetPeriod();
        period = (period & 0xFF) | highTimerBits << 8; // Set the high 3 bits (keeping the low 8 bits)
        _timer.SetPeriod(period);

        const int lenghtCounterLoad = BitwiseUtils::GetBitRange(value, 7, 5);
        _lengthCounter.SetLoad(lenghtCounterLoad);

        // From NESdev: 
        // The sequencer is immediately restarted at the first value of the current sequence. 
        // The envelope is also restarted. 
        // The period divider is not reset.
        _sequencerStep = 0;
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

void SquareChannel::SetEnable(bool enable)
{
    _lengthCounter.SetEnabled(enable);
}

void SquareChannel::Tick()
{
    if (_timer.Tick())
    {
        ++_sequencerStep;
        if (_sequencerStep > 7)
        {
            _sequencerStep = 0;
        }
    }
}

void SquareChannel::QuarterFrameTick()
{
    _envelope.Tick();
}

void SquareChannel::HalfFrameTick()
{
    _lengthCounter.Tick();
    _sweep.Tick(_timer);
}

int SquareChannel::GetOutput() const
{
    // http://wiki.nesdev.com/w/index.php/APU_Pulse
    static const int dutyValues[4][8] = {
        { 0, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 0, 0, 0 },
        { 1, 0, 0, 1, 1, 1, 1, 1 }
    };

    const int envelopeOutput = _envelope.GetOutput();

    // From http://wiki.nesdev.com/w/index.php/APU_Pulse:
    // The mixer receives the current envelope volume except when:
    // The sequencer output is zero, or
    // overflow from the sweep unit's adder is silencing the channel, or
    // the length counter is zero, or
    // the timer has a value less than eight.
    if (dutyValues[_duty][_sequencerStep] == 0)
    {
        return 0;
    }
    if (_sweep.HasOverflowed())
    {
        return 0;
    }
    else if (_lengthCounter.IsZero())
    {
        return 0;
    }
    else if (_timer.GetValue() < 8)
    {
        return 0;
    }
    else
    {
        return envelopeOutput;
    }
}
