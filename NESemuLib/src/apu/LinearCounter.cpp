#include "LinearCounter.h"

void LinearCounter::SetParameters(bool controlFlag, int loadValue)
{
    _controlFlag = controlFlag;
    _reloadValue = loadValue;
}

void LinearCounter::SetReloadFlag()
{
    _reloadFlag = true;
}

void LinearCounter::Tick()
{
    if (_reloadFlag)
    {
        _counter = _reloadValue;
    }
    else if (_counter > 0)
    {
        --_counter;
    }

    if (!_controlFlag)
    {
        _reloadFlag = false;
    }
}

bool LinearCounter::IsZero() const
{
    return _counter == 0;
}
