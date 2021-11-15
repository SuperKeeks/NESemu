#pragma once

#include "MemoryHandler.h"

class RAM : public MemoryHandler
{
public:
    static const int kRAMSize = 2048;

    struct RAMState
    {
        uint8_t _ram[kRAMSize];
    };

    RAM();
    virtual ~RAM();

    RAMState GetSnapshot() { return _state; }
    void LoadSnapshot(RAMState snapshot) { _state = snapshot; }

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

private:
    RAMState _state;
    
    uint16_t ConvertToRealAddress(uint16_t address) const;
};
