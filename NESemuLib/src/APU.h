#pragma once

#include "MemoryHandler.h"

class CPU;

class APU : public MemoryHandler
{
public:
    static const int kFrameCounterFrequency = 240; // Hz

    APU();
    virtual ~APU();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset(CPU* cpu);

    void Tick();

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

    CPU* m_cpu;
    FrameCounterMode m_frameCounterMode;
    bool m_irqEnabled;
    uint8_t m_frameCounter;
};
