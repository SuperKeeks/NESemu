#include "DMCChannel.h"

#include "BitwiseUtils.h"

void DMCChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4010)
    {
        // TODO
        const bool irqEnabled = BitwiseUtils::GetBitRange(value, 7, 1) == 1;
        const bool loopFlag = BitwiseUtils::GetBitRange(value, 6, 1) == 1;

        static const int rateTable[16] = { 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54 };
        const int rateIndex = BitwiseUtils::GetBitRange(value, 4, 4);
        _timer.SetPeriod(rateTable[rateIndex]);
    }
    else if (address == 0x4011)
    {
        _outputLevel = BitwiseUtils::GetBitRange(value, 6, 7);
    }
    else if (address == 0x4012)
    {
        // TODO
        const int sampleAddress = 0xC000 + value * 64;
    }
    else if (address == 0x4013)
    {
        // TODO
        const int sampleLength = value * 16 + 1;
    }
}

void DMCChannel::PowerOn()
{
    _outputLevel = 0;
    _remainingBits = 0;
    _silenceFlag = true;
    _rightShiftRegister = 0;
    _sampleBuffer = 0;
}

void DMCChannel::Reset()
{
}

void DMCChannel::SetEnable(bool enable)
{
    // TODO
    // If the DMC bit is clear, the DMC bytes remaining will be set to 0 and the DMC will silence when it empties.
    // If the DMC bit is set, the DMC sample will be restarted only if its bytes remaining is 0. If there are bits remaining in the 1 - byte sample buffer, these will finish playing before the next sample is fetched.
}

void DMCChannel::Tick()
{
    if (_timer.Tick())
    {
        // From: https://wiki.nesdev.com/w/index.php/APU_DMC
        // When the timer outputs a clock, the following actions occur in order:

        // 1. If the silence flag is clear, the output level changes based on bit 0 of the shift register. 
        // If the bit is 1, add 2; otherwise, subtract 2. But if adding or subtracting 2 would cause the 
        // output level to leave the 0-127 range, leave the output level unchanged. 
        // This means subtract 2 only if the current level is at least 2, or add 2 only if the current level is at most 125.
        if (!_silenceFlag)
        {
            if (BitwiseUtils::GetBitRange(_rightShiftRegister, 0, 1) == 1)
            {
                if (_outputLevel + 2 < 128)
                {
                    _outputLevel += 2;
                }
            }
            else
            {
                if (_outputLevel - 2 >= 0)
                {
                    _outputLevel -= 2;
                }
            }
        }

        // 2. The right shift register is clocked.
        _rightShiftRegister = _rightShiftRegister >> 1;

        // 3. The bits-remaining counter is decremented. If it becomes zero, a new output cycle is started.
        --_remainingBits;
        if (_remainingBits == 0)
        {
            _remainingBits = 8;
            if (_sampleBuffer == 0)
            {
                _silenceFlag = true;
            }
            else
            {
                _silenceFlag = false;
                _rightShiftRegister = _sampleBuffer;
                _sampleBuffer = 0;
            }
        }
    }
}

void DMCChannel::QuarterFrameTick()
{
}

void DMCChannel::HalfFrameTick()
{
}

int DMCChannel::GetOutput() const
{
    OMBAssert(_outputLevel < 0x80, "Output level should be 7-bit!");
    return _outputLevel;
}
