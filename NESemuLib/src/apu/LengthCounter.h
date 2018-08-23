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
    const int loadValues[32] = {
        10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };

    bool _enabled;
    bool _halt;
    int _value;
    int _counter;
};
