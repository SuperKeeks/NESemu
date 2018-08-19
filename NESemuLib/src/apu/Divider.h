#pragma once

class Divider
{
public:
    Divider();
    ~Divider();

    int GetPeriod() const;
    void SetPeriod(int period);
    void ReloadCounter();
    bool Tick(); // Returns true when generating an output clock

private:
    int _period;
    int _counter;
};
