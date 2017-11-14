#pragma once

#include "APU.h"
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

private:
	INESParser _parser;
    RAM _ram;
	PPU _ppu;
	APU _apu;
	Input _input;
	MM5 _mm5;
	SRAM _sram;
	ROM _rom;

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void Reset();

	MemoryHandler& GetMemoryHandlerForAddress(uint16_t address);
};
