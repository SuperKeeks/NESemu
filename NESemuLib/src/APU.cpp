#include "APU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "CPU.h"
#include "LogUtils.h"

APU::APU()
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
        // TODO
        /*bool enablePulse1 = BitwiseUtils::IsFlagSet(value, StatusFlags::EnablePulse1);
        bool enablePulse2 = BitwiseUtils::IsFlagSet(value, StatusFlags::EnablePulse2);
        bool enableTriangle = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableTriangle);
        bool enableNoise = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableNoise);
        bool enableDMC = BitwiseUtils::IsFlagSet(value, StatusFlags::EnableDMC);*/
    }
    else if (address == 0x4017)
    {
        m_frameCounterMode = BitwiseUtils::IsFlagSet(value, FrameCounterFlags::Mode) ? FrameCounterMode::FiveStep : FrameCounterMode::FourStep;
        m_irqEnabled = BitwiseUtils::IsFlagSet(value, FrameCounterFlags::IRQInhibit) ? false : true;
        m_frameCounter = 0;
    }
    else
    {
        OMBAssert(false, "Unimplemented");
    }
}

void APU::PowerOn()
{
    // NOTE: Not sure what these values are supposed to be
    m_frameCounterMode = FrameCounterMode::FourStep;
    m_irqEnabled = false;
    m_frameCounter = 0;
}

void APU::Reset(CPU* cpu)
{
    m_cpu = cpu;
    PowerOn();
}

void APU::Tick()
{
    ++m_frameCounter;
    if (m_frameCounter == 4 && m_frameCounterMode == FrameCounterMode::FourStep && m_irqEnabled)
    {
        m_cpu->ExecuteIRQ();
    }

    // Reset frame counter
    if (m_frameCounter > 5 || (m_frameCounter == 4 && m_frameCounterMode == FrameCounterMode::FourStep))
    {
        m_frameCounter = 0;
    }
}
