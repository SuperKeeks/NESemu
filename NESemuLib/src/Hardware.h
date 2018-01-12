#pragma once

#include "APU.h"
#include "CHRROM.h"
#include "CPU.h"
#include "Input.h"
#include "MM5.h"
#include "PPU.h"
#include "RAM.h"
#include "ROM.h"
#include "SRAM.h"

struct Hardware
{
    CPU cpu;
    RAM ram;
    PPU ppu;
    APU apu;
    Input input;
    MM5 mm5;
    SRAM sram;
    ROM rom;
    CHRROM chrRom;

    void PowerOn()
    {
        cpu.PowerOn();
        ram.PowerOn();
        ppu.PowerOn();
        apu.PowerOn();
        input.PowerOn();
        mm5.PowerOn();
        sram.PowerOn();
        rom.PowerOn();
        chrRom.PowerOn();
    }
    
    void Reset(MemoryMapper* memoryMapper, MirroringMode mirroringMode)
    {
        cpu.Reset(memoryMapper);
        ram.Reset();
        ppu.Reset(memoryMapper, &cpu, &chrRom, mirroringMode);
        apu.Reset();
        input.Reset();
        mm5.Reset();
        sram.Reset();
        rom.Reset();
        chrRom.Reset();
    }
};