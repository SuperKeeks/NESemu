#include "Divider.h"

Divider::Divider() : 
    _period(0),
    _counter(0)
{
}

Divider::~Divider()
{
}

int Divider::GetPeriod() const
{
    return _period;
}

void Divider::SetPeriod(int period)
{
    _period = period;
}

void Divider::Reset()
{
    _counter = _period;
}

bool Divider::Tick()
{
    if (_counter == 0)
    {
        Reset();
        return true;
    }
    else
    {
        --_counter;
        return false;
    }
}

int Divider::GetValue() const
{
    return _counter;
}
