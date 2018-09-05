#include "NoiseChannel.h"

#include "BitwiseUtils.h"

void NoiseChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x400C)
    {
        const bool lengthCounterHaltEnvelopeLoopFlag = BitwiseUtils::GetBitRange(value, 5, 1) == 1;
        _lengthCounter.SetHalt(lengthCounterHaltEnvelopeLoopFlag);

        const bool constantVolumeEnvelopeFlag = BitwiseUtils::GetBitRange(value, 4, 1) == 1;
        const int volumeEnvelopeDividerPeriod = BitwiseUtils::GetBitRange(value, 3, 4);
        _envelope.SetParameters(lengthCounterHaltEnvelopeLoopFlag, constantVolumeEnvelopeFlag, volumeEnvelopeDividerPeriod);
    }
    else if (address == 0x400D)
    {
        // Unused
    }
    else if (address == 0x400E)
    {
        _modeFlag = BitwiseUtils::GetBitRange(value, 7, 1);

        const int periodValues[] = { 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 };
        const int periodIndex = BitwiseUtils::GetBitRange(value, 3, 4);
        _timer.SetPeriod(periodValues[periodIndex]);
    }
    else if (address == 0x400F)
    {
        const int lengthCounterLoad = BitwiseUtils::GetBitRange(value, 7, 5);
        _lengthCounter.SetLoad(lengthCounterLoad);
        
        _envelope.SetStartFlag();
    }
    else
    {
        OMBAssert(false, "Invalid address!");
    }
}

void NoiseChannel::PowerOn()
{
    _shiftRegister = 1;
    Reset();
}

void NoiseChannel::Reset()
{
    _modeFlag = false;
}

void NoiseChannel::SetEnable(bool enable)
{
    _lengthCounter.SetEnabled(enable);
}

void NoiseChannel::Tick()
{
    if (_timer.Tick())
    {
        // From NESdev:
        // When the timer clocks the shift register, the following actions occur in order:
        //  1. Feedback is calculated as the exclusive - OR of bit 0 and one other bit: bit 6 if Mode flag is set, otherwise bit 1.
        //  2. The shift register is shifted right by one bit.
        //  3. Bit 14, the leftmost bit, is set to the feedback calculated earlier.
        const int otherBit = _modeFlag ? BitwiseUtils::GetBitRange(_shiftRegister, 6, 1) : BitwiseUtils::GetBitRange(_shiftRegister, 1, 1);
        const int feedback = BitwiseUtils::GetBitRange(_shiftRegister, 0, 1) ^ otherBit;
        _shiftRegister = _shiftRegister >> 1;
        _shiftRegister = _shiftRegister | (feedback << 14);
    }
}

void NoiseChannel::QuarterFrameTick()
{
    _envelope.Tick();
}

void NoiseChannel::HalfFrameTick()
{
    _lengthCounter.Tick();
}

int NoiseChannel::GetOutput() const
{
    // The mixer receives the current envelope volume except when:
    //  - Bit 0 of the shift register is set, or
    //  - The length counter is zero
    if (BitwiseUtils::GetBitRange(_shiftRegister, 0, 1) == 1)
    {
        return 0;
    }
    else if (_lengthCounter.IsZero())
    {
        return 0;
    }
    else
    {
        return _envelope.GetOutput();
    }
}
