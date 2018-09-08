#pragma once

#include "MemoryHandler.h"

#include "apu/DMCChannel.h"
#include "apu/NoiseChannel.h"
#include "apu/SquareChannel.h"
#include "apu/TriangleChannel.h"
#include "RingBuffer.h"

class CPU;
class MemoryMapper;

class APU : public MemoryHandler
{
public:
    static const uint64_t kBufferSize = 1024;

    APU();
    virtual ~APU();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset(CPU* cpu, MemoryMapper* memoryMapper);

    void SetOutputFrequency(int frequency);
    void Tick();
    RingBuffer<double, kBufferSize>& GetBuffer();
    int GetBufferFilledLength() const;
    void ClearBuffer();

private:
    enum StatusFlags
    {
        EnablePulse1 = 0,
        EnablePulse2,
        EnableTriangle,
        EnableNoise,
        EnableDMC
    };

    enum FrameCounterFlags
    {
        IRQInhibit = 6,
        Mode = 7
    };

    enum FrameCounterMode
    {
        FourStep,
        FiveStep
    };

    CPU* _cpu;
    FrameCounterMode _frameCounterMode;
    bool _frameInterruptFlag;
    bool _irqEnabled;
    int _cpuCycles;
    bool _isEvenCPUCycle;
    RingBuffer<double, kBufferSize> _outputBuffer;
    double _cyclesPerSample;
    double _cyclesSinceLastSample;

    // Channels
    SquareChannel _squareChannel1;
    SquareChannel _squareChannel2;
    TriangleChannel _triangleChannel;
    NoiseChannel _noiseChannel;
    DMCChannel _dmcChannel;

    void ResetCPUCycles();
    void TrySetInterruptFlag();
    void QuarterFrameTick();
    void HalfFrameTick();
    double GenerateSample();
};
