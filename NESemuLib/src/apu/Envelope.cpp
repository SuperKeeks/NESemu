#include "Envelope.h"

// Reference: http://wiki.nesdev.com/w/index.php/APU_Envelope

Envelope::Envelope() : 
    _startFlag(false),
    _loopFlag(false),
    _constantVolumeFlag(false),
    _decayLevelCounter(0),
    _constantVolumeValue(0)
{
}

Envelope::~Envelope()
{
}

void Envelope::SetParameters(bool loopFlag, bool constantVolumeFlag, uint8_t volumeAndDividerPeriod)
{
    _loopFlag = loopFlag;
    _constantVolumeFlag = constantVolumeFlag;
    _constantVolumeValue = volumeAndDividerPeriod;
    _divider.SetPeriod(volumeAndDividerPeriod);
}

void Envelope::SetStartFlag()
{
    _startFlag = true;
}

void Envelope::Tick()
{
    if (_startFlag)
    {
        _startFlag = false;
        _decayLevelCounter = kDecayLevelCounterReloadValue;
        _divider.ReloadCounter();
    }
    else
    {
        if (_divider.Tick())
        {
            if (_decayLevelCounter > 0)
            {
                --_decayLevelCounter;
            }
            else if (_loopFlag)
            {
                _decayLevelCounter = kDecayLevelCounterReloadValue;
            }
        }
    }
}

int Envelope::GetOutput() const
{
    if (_constantVolumeFlag)
    {
        return _constantVolumeValue;
    }
    else
    {
        return _decayLevelCounter;
    }
}
