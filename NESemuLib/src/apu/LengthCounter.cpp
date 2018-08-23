#include "LengthCounter.h"

#include "Assert.h"

LengthCounter::LengthCounter() :
    _halt(true),
    _value(0),
    _counter(0)
{
}

void LengthCounter::Tick()
{
    if (_counter > 0 && !_halt)
    {
        --_counter;
    }
}

void LengthCounter::SetEnabled(bool enabled)
{
    if (!enabled)
    {
        _counter = 0;
    }
    _enabled = enabled;
}

void LengthCounter::SetHalt(bool halt)
{
    _halt = halt;
}

void LengthCounter::SetLoad(int loadIndex)
{
    OMBAssert(loadIndex >= 0 && loadIndex <= 0x1F, "Invalid load value");
    if (_enabled)
    {
        _counter = loadValues[loadIndex] + 1;
    }
}

bool LengthCounter::IsZero() const
{
    return _counter == 0;
}
