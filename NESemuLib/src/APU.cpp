#include "APU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "CPU.h"
#include "LogUtils.h"
#include "SizeOfArray.h"

APU::APU() : 
    _squareChannel1(1),
    _squareChannel2(2),
    _cyclesPerSample(0)
{
}

APU::~APU()
{
}

uint8_t APU::ReadMem(uint16_t address)
{
    OMBAssert(false, "Unimplemented");
    return 0;
}

void APU::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x4015)
    {
        const bool enablePulse1 = BitwiseUtils::IsFlagSet(value, StatusFlags::EnablePulse1);
        _squareChannel1.SetEnable(enablePulse1);
        const bool enablePulse2 = BitwiseUtils::IsFlagSet(value, StatusFlags::EnablePulse2);
        _squareChannel2.SetEnable(enablePulse2);
        const bool enableTriangle = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableTriangle);
        _triangleChannel.SetEnable(enableTriangle);
        // TODO
        /*bool enableNoise = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableNoise);
        bool enableDMC = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableDMC);*/
    }
    else if (address == 0x4017)
    {
        _frameCounterMode = BitwiseUtils::IsFlagSet(value, FrameCounterFlags::Mode) ? FrameCounterMode::FiveStep : FrameCounterMode::FourStep;
        _irqEnabled = BitwiseUtils::IsFlagSet(value, FrameCounterFlags::IRQInhibit) ? false : true;
        ResetCPUCycles();

        if (_frameCounterMode == FrameCounterMode::FiveStep)
        {
            QuarterFrameTick();
            HalfFrameTick();
        }
    }
    else if (address >= 0x4000 && address <= 0x4003)
    {
        _squareChannel1.WriteMem(address, value);
    }
    else if (address >= 0x4004 && address <= 0x4007)
    {
        _squareChannel2.WriteMem(address, value);
    }
    else if (address >= 0x4008 && address <= 0x400B)
    {
        _triangleChannel.WriteMem(address, value);
    }
    else if (address >= 0x400C && address <= 0x400F)
    {
        // TODO: Write to Noise channel
    }
    else if (address >= 0x4010 && address <= 0x4013)
    {
        // TODO: Write to DMC channel
    }
    else
    {
        OMBAssert(false, "Unimplemented");
    }
}

void APU::PowerOn()
{
    _squareChannel1.PowerOn();
    _squareChannel2.PowerOn();
    _triangleChannel.PowerOn();
    // TODO: Noise
    // TODO: DMC
}

void APU::Reset(CPU* cpu)
{
    _cpu = cpu;
    _frameCounterMode = FrameCounterMode::FourStep;
    _irqEnabled = false;
    _isEvenCPUCycle = false;
    _nextBufferIndex = 0;
    _cyclesSinceLastSample = 0;
    ResetCPUCycles();

    _squareChannel1.Reset();
    _squareChannel2.Reset();
    _triangleChannel.Reset();
    // TODO: Noise
    // TODO: DMC

    WriteMem(0x4015, 0); // "Power-up and reset have the effect of writing $00, silencing all channels."
}

void APU::SetOutputFrequency(int frequency)
{
    _cyclesPerSample = (21477272 / 12) / frequency; // CPU frequency divided by output frequency
}

void APU::Tick()
{
    OMBAssert(_cyclesPerSample > 0, "Output frequency hasn't been set");

    ++_cpuCycles;
    _isEvenCPUCycle = !_isEvenCPUCycle;

    // Timer ticking
    _triangleChannel.Tick();
    if (_isEvenCPUCycle)
    {
        _squareChannel1.Tick();
        _squareChannel2.Tick();
        // TODO: Noise Tick()
        // TODO: DMC Tick()
    }

    // Frame counter update
    // Reference: http://wiki.nesdev.com/w/index.php/APU_Frame_Counter
    switch (_cpuCycles)
    {
        case 7457: // 3728.5 * 2
        {
            QuarterFrameTick();
            break;
        }

        case 14913: // 7456.5 * 2
        {
            QuarterFrameTick();
            HalfFrameTick();
            break;
        }

        case 22371: // 11185.5 * 2
        {
            QuarterFrameTick();
            break;
        }

        case 29828: // 14914 * 2
        {
            TrySetInterruptFlag();
            break;
        }

        case 29829: // 14914.5 * 2
        {
            if (_frameCounterMode == FrameCounterMode::FourStep)
            {
                QuarterFrameTick();
                HalfFrameTick();
                TrySetInterruptFlag();
            }
            break;
        }

        case 29830: // 14915 * 2
        {
            if (_frameCounterMode == FrameCounterMode::FourStep)
            {
                TrySetInterruptFlag();
                ResetCPUCycles();
            }
            break;
        }

        case 37281: // 18640.5 * 2
        {
            OMBAssert(_frameCounterMode == FrameCounterMode::FiveStep, "Unexpected frame counter value for FourStep mode!");
            QuarterFrameTick();
            HalfFrameTick();
            ResetCPUCycles();
            break;
        }
    }

    ++_cyclesSinceLastSample;
    if (_cyclesSinceLastSample >= _cyclesPerSample)
    {
        _cyclesSinceLastSample = 0;
        OMBAssert(_nextBufferIndex < sizeofarray(_outputBuffer), "Buffer is filled!");
        if (_nextBufferIndex < sizeofarray(_outputBuffer))
        {
            _outputBuffer[_nextBufferIndex] = GenerateSample();
            ++_nextBufferIndex;
        }
    }
}

double* APU::GetBuffer()
{
    return _outputBuffer;
}

int APU::GetBufferFilledLength() const
{
    return _nextBufferIndex;
}

void APU::ClearBuffer()
{
    _nextBufferIndex = 0;
}

void APU::ResetCPUCycles()
{
    _cpuCycles = -1; // -1 so on the next Tick() it gets to 0
}

void APU::TrySetInterruptFlag()
{
    if (_frameCounterMode == FrameCounterMode::FourStep && _irqEnabled)
    {
        _cpu->ExecuteIRQ();
    }
}

void APU::QuarterFrameTick()
{
    _squareChannel1.QuarterFrameTick();
    _squareChannel2.QuarterFrameTick();
    _triangleChannel.QuarterFrameTick();
    // TODO: Noise
    // TODO: DMC
}

void APU::HalfFrameTick()
{
    QuarterFrameTick();
    _squareChannel1.HalfFrameTick();
    _squareChannel2.HalfFrameTick();
    _triangleChannel.HalfFrameTick();
    // TODO: Noise
    // TODO: DMC
}

double APU::GenerateSample()
{
    // Reference: http://wiki.nesdev.com/w/index.php/APU_Mixer
    // NOTE: This can be optimised by using Lookup tables or a linear approximation (check APU Mixer reference)

    // Pulse output
    double pulseOutput = 0;
    const int square1Output = _squareChannel1.GetOutput();
    const int square2Output = _squareChannel2.GetOutput();
    if (square1Output != 0 || square2Output != 0)
    {
        pulseOutput = 95.88 / ((8128.0 / (square1Output + square2Output)) + 100);
    }

    // Triangle, Noise and DMC outputs
    double tndOutput = 0;
    const double triangleOutput = _triangleChannel.GetOutput();
    const double noiseOutput = 0; // TODO
    const double dmcOutput = 0; // TODO
    if (triangleOutput != 0 || noiseOutput != 0 || dmcOutput != 0)
    {
        tndOutput = 159.79 / ((1.0 / (triangleOutput / 8227.0 + noiseOutput / 12241.0 + dmcOutput / 22638.0)) + 100);
    }

    return pulseOutput + tndOutput;
}
