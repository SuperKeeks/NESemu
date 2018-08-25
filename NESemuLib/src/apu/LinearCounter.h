#pragma once

class LinearCounter
{
public:
    void SetParameters(bool controlFlag, int loadValue);
    void SetReloadFlag();
    void Tick();
    bool IsZero() const;

private:
    int _counter = 0;
    bool _controlFlag = false;
    int _reloadValue = 0;
    bool _reloadFlag = false;
};
