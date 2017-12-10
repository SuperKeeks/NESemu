#pragma once

#include "APU.h"
#include "CHRROM.h"
#include "CPU.h"
#include "INESParser.h"
#include "Input.h"
#include "MM5.h"
#include "PPU.h"
#include "RAM.h"
#include "ROM.h"
#include "SRAM.h"

#include <stdint.h>

class NESemu : public MemoryHandler
{
public:
    NESemu();
    ~NESemu();

    void Load(const char* path);
    void Load(const uint8_t rom[], uint16_t romSize); // For testing purposes
    void Update();
    virtual void Reset();

    CPU* GetCPU() { return &_cpu; }

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);

private:
    CPU _cpu;
    INESParser _parser;
    RAM _ram;
    PPU _ppu;
    APU _apu;
    Input _input;
    MM5 _mm5;
    SRAM _sram;
    ROM _rom;
    CHRROM _chrRom;

    MemoryHandler& GetMemoryHandlerForAddress(uint16_t address);
};
