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
    static const int loadValues[32] = {
        10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };

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
