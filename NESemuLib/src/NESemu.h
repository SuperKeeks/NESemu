#pragma once

#include "PPU.h"
#include "RAM.h"

#include <stdint.h>

class NESemu : public MemoryHandler
{
public:
	NESemu();
	~NESemu();

private:
    RAM _ram;
	PPU _ppu;

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void Reset();

	MemoryHandler& GetMemoryHandlerForAddress(uint16_t address);
};
