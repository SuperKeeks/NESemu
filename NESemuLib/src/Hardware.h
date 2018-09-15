#pragma once

#include "APU.h"
#include "CPU.h"
#include "Input.h"
#include "PPU.h"
#include "RAM.h"
#include "SRAM.h"

struct Hardware
{
    CPU cpu;
    RAM ram;
    PPU ppu;
    APU apu;
    Input input;
    SRAM sram;

    void PowerOn()
    {
        cpu.PowerOn();
        ram.PowerOn();
        ppu.PowerOn();
        apu.PowerOn();
        input.PowerOn();
        sram.PowerOn();
    }
    
    void Reset(MemoryMapper* memoryMapper, MirroringMode mirroringMode)
    {
        cpu.Reset(memoryMapper);
        ram.Reset();
        ppu.Reset(memoryMapper, &cpu, mirroringMode);
        apu.Reset(&cpu, memoryMapper);
        input.Reset();
        sram.Reset();
    }
};