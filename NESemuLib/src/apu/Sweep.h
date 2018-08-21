#pragma once

#include "Divider.h"

class Sweep
{
public:
    Sweep();

    void SetParameters(int channelNumber, bool enabled, int period, bool negateFlag, int shiftCount);
    void Tick(Divider& channelTimer);
    bool HasOverflowed() const;

private:
    int _channelNumber;
    Divider _divider;
    bool _enabled;
    int _period;
    bool _negateFlag;
    int _shiftCount;
    Divider* _channelTimer;
    bool _reload;
    bool _hasOverflowed;
};
