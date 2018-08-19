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

void Divider::ReloadCounter()
{
    _counter = _period;
}

bool Divider::Tick()
{
    if (_counter == 0)
    {
        ReloadCounter();
        return true;
    }
    else
    {
        --_counter;
        return false;
    }
}
