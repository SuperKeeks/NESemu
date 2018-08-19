#pragma once

#include "MemoryHandler.h"

#include "apu\SquareChannel.h"

class CPU;

class APU : public MemoryHandler
{
public:
    APU();
    virtual ~APU();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset(CPU* cpu);

    void Tick();
    double* GetBuffer();
    int GetBufferFilledLength() const;
    void ClearBuffer();

private:
    enum StatusFlags
    {
        EnablePulse1,
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
    bool _irqEnabled;
    int _cpuCycles;
    bool _isEvenCPUCycle;
    double _outputBuffer[4096];
    int _nextBufferIndex;
    int _cyclesSinceLastSample;

    // Channels
    SquareChannel _squareChannel1;
    SquareChannel _squareChannel2;

    void ResetCPUCycles();
    void TrySetInterruptFlag();
    void QuarterFrameTick();
    void HalfFrameTick();
    double GenerateSample();
};
