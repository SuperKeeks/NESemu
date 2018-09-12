#pragma once

class LengthCounter
{
public:
    LengthCounter();
    void Tick();
    void SetEnabled(bool enabled);
    void SetHalt(bool halt);
    void SetLoad(int loadIndex);
    bool IsZero() const;

private:
    bool _enabled;
    bool _halt;
    int _value;
    int _counter;
};
