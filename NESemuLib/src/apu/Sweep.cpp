#include "Sweep.h"

#include "Assert.h"

// http://wiki.nesdev.com/w/index.php/APU_Sweep

Sweep::Sweep() :
    _channelNumber(0),
    _enabled(false),
    _negateFlag(false),
    _shiftCount(0),
    _reload(false),
    _hasOverflowed(false)
{
}

void Sweep::SetParameters(int channelNumber, bool enabled, int period, bool negateFlag, int shiftCount)
{
    OMBAssert(channelNumber == 1 || channelNumber == 2, "Invalid channel number!");
    _channelNumber = channelNumber;
    _enabled = enabled;
    _period = period + 1;
    _negateFlag = negateFlag;
    _shiftCount = shiftCount;
    _reload = true;
}

void Sweep::Tick(Divider& channelTimer)
{
    const bool outputClock = _divider.Tick();
    if (outputClock)
    {
        int changeAmount = channelTimer.GetPeriod() >> _shiftCount;
        if (_negateFlag)
        {
            changeAmount = -changeAmount;
            if (_channelNumber == 1)
            {
                changeAmount -= 1;
            }
        }

        const int result = channelTimer.GetPeriod() + changeAmount;
        if (result > 0x7FF)
        {
            _hasOverflowed = true;
        }
        else if (_enabled && _shiftCount > 0)
        {
            channelTimer.SetPeriod(result);
        }
    }
    
    if (_reload || outputClock)
    {
        _divider.SetPeriod(_period);
        _divider.Reset();
        _hasOverflowed = false;
        _reload = false;
    }
}

bool Sweep::HasOverflowed() const
{
    return _hasOverflowed;
}
