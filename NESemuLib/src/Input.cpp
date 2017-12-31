#include "Input.h"

#include "Assert.h"
#include "SizeOfArray.h"

Input::Input()
{
}

Input::~Input()
{
}

uint8_t Input::ReadMem(uint16_t address)
{
    if (address == kController1Address)
    {
        if (_current4016Index >= sizeofarray(_4016))
        {
            OMBAssert(false, "$4016 input read out of bounds!");
            _current4016Index = 0;
        }

        uint8_t value = _4016[_current4016Index];
        ++_current4016Index;
        return value;
    }
    else if (address == kController2Address)
    {
        if (_current4017Index >= sizeofarray(_4017))
        {
            OMBAssert(false, "$4017 input read out of bounds!");
            _current4017Index = 0;
        }

        uint8_t value = _4017[_current4017Index];
        ++_current4017Index;
        return value;
    }
    else
    {
        OMBAssert(false, "Address %#06x is not part of Input!", address);
    }

    return 0;
}

void Input::WriteMem(uint16_t address, uint8_t value)
{
    OMBAssert(address == kController1Address, "Address %#06x is not part of Input!", address);
    if (_lastWrittenValue == 0x1 && value == 0x0)
    {
        _current4016Index = 0;
        _current4017Index = 0;
    }
    _lastWrittenValue = value;
}

void Input::PowerOn()
{
    Reset();
}

void Input::Reset()
{
    for (int i = 0; i < sizeofarray(_4016); ++i)
    {
        _4016[i] = false;
    }
    
    for (int i = 0; i < sizeofarray(_4017); ++i)
    {
        _4017[i] = false;
    }

    _current4016Index = 0;
    _current4017Index = 0;
    _lastWrittenValue = 0x0;
}

void Input::SetControllerState(int controllerNumber, const ControllerState& state)
{
    bool* curPos;
    if (controllerNumber == 1)
    {
        curPos = _4016;
        _4016[19] = true; // Signature
    }
    else if (controllerNumber == 2)
    {
        curPos = _4017;
        _4017[18] = true; // Signature
    }
    else if (controllerNumber == 3)
    {
        curPos = _4016 + 8;
    }
    else if (controllerNumber == 4)
    {
        curPos = _4017 + 8;
    }
    else
    {
        OMBAssert(false, "Unsupported controller number %d", controllerNumber);
        return;
    }

    *curPos++ = state.A;
    *curPos++ = state.B;
    *curPos++ = state.Select;
    *curPos++ = state.Start;
    *curPos++ = state.Up;
    *curPos++ = state.Down;
    *curPos++ = state.Left;
    *curPos++ = state.Right;
}
