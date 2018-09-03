#include "DMCChannel.h"

#include "BitwiseUtils.h"

void DMCChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4010)
    {
        // TODO
        const bool irqEnabled = BitwiseUtils::GetBitRange(value, 7, 1) == 1;
        const bool loopFlag = BitwiseUtils::GetBitRange(value, 6, 1) == 1;
        const int rateIndex = BitwiseUtils::GetBitRange(value, 4, 4);
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
