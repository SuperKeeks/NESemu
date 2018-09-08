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
    if (address == 0x4015)
    {
        const int squareChannel1Bit = _squareChannel1.IsLengthCounterZero() ? 0 : 1;
        const int squareChannel2Bit = _squareChannel2.IsLengthCounterZero() ? 0 : 1;
        const int triangleChannelBit = _triangleChannel.IsLengthCounterZero() ? 0 : 1;
        const int noiseChannelBit = _noiseChannel.IsLengthCounterZero() ? 0 : 1;
        const int dmcChannelBit = _dmcChannel.IsBytesRemainingZero() ? 0 : 1;
        const int frameInterruptFlagBit = _frameInterruptFlag ? 1 : 0;
        const int dmcInterruptFlagBit = _dmcChannel.IsInterruptFlagSet() ? 1 : 0;

        const int value =
            squareChannel1Bit << 0 |
            squareChannel2Bit << 1 |
            triangleChannelBit << 2 |
            noiseChannelBit << 3 |
            dmcChannelBit << 4 |
            frameInterruptFlagBit << 6 |
            dmcInterruptFlagBit << 7;

        // "Reading this register clears the frame interrupt flag (but not the DMC interrupt flag)."
        _frameInterruptFlag = false;

        return value;
    }
    else
    {
        OMBAssert(false, "Unimplemented");
        return 0;
    }
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
        const bool enableNoise = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableNoise);
        _noiseChannel.SetEnable(enableNoise);
        const bool enableDMC = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableDMC);
        _dmcChannel.SetEnable(enableDMC);
    }
    else if (address == 0x4017)
    {
        _frameCounterMode = BitwiseUtils::IsFlagSet(value, FrameCounterFlags::Mode) ? FrameCounterMode::FiveStep : FrameCounterMode::FourStep;
        _irqEnabled = BitwiseUtils::IsFlagSet(value, FrameCounterFlags::IRQInhibit) ? false : true;
        if (!_irqEnabled)
        {
            _frameInterruptFlag = false;
        }
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
        _noiseChannel.WriteMem(address, value);
    }
    else if (address >= 0x4010 && address <= 0x4013)
    {
        _dmcChannel.WriteMem(address, value);
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
    _noiseChannel.PowerOn();
    _dmcChannel.PowerOn();
}

void APU::Reset(CPU* cpu, MemoryMapper* memoryMapper)
{
    _cpu = cpu;
    _frameCounterMode = FrameCounterMode::FourStep;
    _frameInterruptFlag = false;
    _irqEnabled = false;
    _isEvenCPUCycle = false;
    _outputBuffer.Reset();
    _cyclesSinceLastSample = 0;
    ResetCPUCycles();

    _squareChannel1.Reset();
    _squareChannel2.Reset();
    _triangleChannel.Reset();
    _noiseChannel.Reset();
    _dmcChannel.Reset(memoryMapper);

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
        _noiseChannel.Tick();
        _dmcChannel.Tick();
        if (_dmcChannel.IsInterruptFlagSet())
        {
            // "At any time, if the interrupt flag is set, the CPU's IRQ line is continuously asserted 
            // until the interrupt flag is cleared. The processor will continue on from where it was stalled."
            _cpu->ExecuteIRQ();
        }
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
        OMBAssert(_outputBuffer.IsFull(), "Buffer is full!");
        _outputBuffer.Write(GenerateSample());
    }

    if (_frameInterruptFlag)
    {
        _cpu->ExecuteIRQ();
    }
}

RingBuffer<double, APU::kBufferSize>& APU::GetBuffer()
{
    return _outputBuffer;
}

void APU::ResetCPUCycles()
{
    _cpuCycles = -1; // -1 so on the next Tick() it gets to 0
}

void APU::TrySetInterruptFlag()
{
    if (_frameCounterMode == FrameCounterMode::FourStep && _irqEnabled)
    {
        _frameInterruptFlag = true;
    }
}

void APU::QuarterFrameTick()
{
    _squareChannel1.QuarterFrameTick();
    _squareChannel2.QuarterFrameTick();
    _triangleChannel.QuarterFrameTick();
    _noiseChannel.QuarterFrameTick();
    _dmcChannel.QuarterFrameTick();
}

void APU::HalfFrameTick()
{
    QuarterFrameTick();
    _squareChannel1.HalfFrameTick();
    _squareChannel2.HalfFrameTick();
    _triangleChannel.HalfFrameTick();
    _noiseChannel.HalfFrameTick();
    _dmcChannel.HalfFrameTick();
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
    const double noiseOutput = _noiseChannel.GetOutput();
    const double dmcOutput = _dmcChannel.GetOutput();
    if (triangleOutput != 0 || noiseOutput != 0 || dmcOutput != 0)
    {
        tndOutput = 159.79 / ((1.0 / (triangleOutput / 8227.0 + noiseOutput / 12241.0 + dmcOutput / 22638.0)) + 100);
    }

    return pulseOutput + tndOutput;
}
