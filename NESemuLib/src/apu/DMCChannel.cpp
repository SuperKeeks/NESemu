#include "DMCChannel.h"

#include "BitwiseUtils.h"
#include "MemoryMapper.h"

void DMCChannel::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4010)
    {
        // "IRQ enabled flag. If clear, the interrupt flag is cleared."
        _irqEnabledFlag = BitwiseUtils::GetBitRange(value, 7, 1) == 1;
        if (!_irqEnabledFlag)
        {
            _interruptFlag = false;
        }
        
        _loopFlag = BitwiseUtils::GetBitRange(value, 6, 1) == 1;

        static const int rateTable[16] = { 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54 };
        const int rateIndex = BitwiseUtils::GetBitRange(value, 3, 4);
        _timer.SetPeriod(rateTable[rateIndex] / 2);
    }
    else if (address == 0x4011)
    {
        _outputLevel = BitwiseUtils::GetBitRange(value, 6, 7);
    }
    else if (address == 0x4012)
    {
        _sampleAddress = 0xC000 + value * 64;
    }
    else if (address == 0x4013)
    {
        _sampleLength = value * 16 + 1;
    }
}

void DMCChannel::PowerOn()
{
    _outputLevel = 0;
    _irqEnabledFlag = false;
    _loopFlag = false;
    _interruptFlag = false;
    _remainingBits = 0;
    _silenceFlag = true;
    _rightShiftRegister = 0;
    _sampleBuffer = 0;
    _sampleBufferEmpty = true;
    _sampleAddress = 0;
    _sampleLength = 0;
    _addressCounter = 0;
    _bytesRemaining = 0;
    _memoryMapper = nullptr;
}

void DMCChannel::Reset(MemoryMapper* memoryMapper)
{
    _memoryMapper = memoryMapper;
}

void DMCChannel::SetEnable(bool enable)
{
    if (!enable)
    {
        // If the DMC bit is clear, the DMC bytes remaining will be set to 0 and the DMC will silence when it empties.
        _bytesRemaining = 0;
    }
    else if (enable && _bytesRemaining == 0)
    {
        // If the DMC bit is set, the DMC sample will be restarted only if its bytes remaining is 0.
        // If there are bits remaining in the 1-byte sample buffer, these will finish playing before the next sample is fetched.
        RestartSample();
    }

    // Writing to $4015 clears the DMC interrupt flag.
    _interruptFlag = false;
}

void DMCChannel::Tick()
{
    TickMemoryReader();
    TickOutputUnit();
}

void DMCChannel::QuarterFrameTick()
{
}

void DMCChannel::HalfFrameTick()
{
}

int DMCChannel::GetOutput() const
{
    OMBAssert(_outputLevel >= 0 && _outputLevel < 0x80, "Output level should be 7-bit!");
    return _outputLevel;
}

bool DMCChannel::IsInterruptFlagSet() const
{
    return _interruptFlag;
}

bool DMCChannel::IsBytesRemainingZero() const
{
    return _bytesRemaining == 0;
}

void DMCChannel::TickMemoryReader()
{
    if (_sampleBufferEmpty && _bytesRemaining > 0)
    {
        // TODO? "The CPU is stalled for up to 4 CPU cycles to allow the longest possible write (the return address and write after an IRQ) to finish. 
        // If OAM DMA is in progress, it is paused for two cycles"

        // "The sample buffer is filled with the next sample byte read from the current address, subject to whatever mapping hardware is present."
        _sampleBuffer = _memoryMapper->ReadMem(_addressCounter);
        _sampleBufferEmpty = false;

        // "The address is incremented; if it exceeds $FFFF, it is wrapped around to $8000."
        ++_addressCounter;
        if (_addressCounter > 0xFFFF)
        {
            _addressCounter = 0x8000;
        }
        
        // "The bytes remaining counter is decremented"
        --_bytesRemaining;
        if (_bytesRemaining == 0)
        {
            if (_loopFlag)
            {
                // "if it becomes zero and the loop flag is set, the sample is restarted(see above)"
                RestartSample();
            }
            else if (_irqEnabledFlag)
            {
                // "Otherwise, if the bytes remaining counter becomes zero and the IRQ enabled flag is set, the interrupt flag is set."
                _interruptFlag = true;
            }
        }
    }
}

void DMCChannel::TickOutputUnit()
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
        if (_remainingBits <= 0)
        {
            _remainingBits = 8;
            if (_sampleBufferEmpty)
            {
                _silenceFlag = true;
            }
            else
            {
                _silenceFlag = false;
                _rightShiftRegister = _sampleBuffer;
                _sampleBuffer = 0;
                _sampleBufferEmpty = true;
                RestartSample();
            }
        }
    }
}

void DMCChannel::RestartSample()
{
    _addressCounter = _sampleAddress;
    _bytesRemaining = _sampleLength;
}
