#pragma once

class Divider
{
public:
    Divider();
    ~Divider();

    int GetPeriod() const;
    void SetPeriod(int period);
    void Reset();
    bool Tick(); // Returns true when generating an output clock
    int GetValue() const;

private:
    int _period;
    int _counter;
};
