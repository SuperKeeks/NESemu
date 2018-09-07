#pragma once

#include "AudioChannel.h"

#include "Divider.h"

class MemoryMapper;

class DMCChannel : public AudioChannel
{
public:
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset(MemoryMapper* memoryMapper);

    virtual void SetEnable(bool enable);
    virtual void Tick();
    virtual void QuarterFrameTick();
    virtual void HalfFrameTick();
    virtual int GetOutput() const;

    bool IsInterruptFlagSet() const;
    bool IsBytesRemainingZero() const;

private:
    Divider _timer;
    int _outputLevel;
    bool _irqEnabledFlag;
    bool _loopFlag;
    bool _interruptFlag;
    int _remainingBits;
    bool _silenceFlag;
    int _rightShiftRegister;
    int _sampleBuffer;
    bool _sampleBufferEmpty;
    int _sampleAddress;
    int _sampleLength;
    int _addressCounter;
    int _bytesRemaining;
    MemoryMapper* _memoryMapper;

    void TickMemoryReader();
    void TickOutputUnit();
    void RestartSample();
};
