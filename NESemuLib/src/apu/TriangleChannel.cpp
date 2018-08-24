#include "TriangleChannel.h"

void TriangleChannel::WriteMem(uint16_t address, uint8_t value)
{
}

void TriangleChannel::PowerOn()
{
}

void TriangleChannel::Reset()
{
}

void TriangleChannel::SetEnable(bool enable)
{
    OMBAssert(false, "Unimplemented!");
}

void TriangleChannel::Tick()
{
}

void TriangleChannel::QuarterFrameTick()
{
}

void TriangleChannel::HalfFrameTick()
{
}

int TriangleChannel::GetOutput() const
{
    return 0;
}
