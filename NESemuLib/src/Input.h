#pragma once

#include "MemoryHandler.h"

class Input : public MemoryHandler
{
public:
    struct ControllerState
    {
        bool A;
        bool B;
        bool Select;
        bool Start;
        bool Up;
        bool Down;
        bool Left;
        bool Right;

        ControllerState()
        {
            A = false;
            B = false;
            Select = false;
            Start = false;
            Up = false;
            Down = false;
            Left = false;
            Right = false;
        }
    };

    static const uint16_t kController1Address = 0x4016;
    static const uint16_t kController2Address = 0x4017;

    Input();
    virtual ~Input();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    void SetControllerState(int controllerNumber, const ControllerState& state);

private:
    bool _4016[24];
    bool _4017[24];
    uint8_t _current4016Index;
    uint8_t _current4017Index;
    uint8_t _4016LastWrittenValue;
    uint8_t _4017LastWrittenValue;
};
