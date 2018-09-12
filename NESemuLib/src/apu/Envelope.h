#pragma once

#include "Divider.h"

#include <stdint.h>

class Envelope
{
public:
    Envelope();
    ~Envelope();

    void SetParameters(bool loopFlag, bool constantVolumeFlag, uint8_t volumeAndDividerPeriod);
    void SetStartFlag();
    void Tick();
    int GetOutput() const;

private:
    static const int kDecayLevelCounterReloadValue = 15;

    bool _startFlag;
    bool _loopFlag;
    bool _constantVolumeFlag;
    int _decayLevelCounter;
    int _constantVolumeValue;
    Divider _divider;
};
