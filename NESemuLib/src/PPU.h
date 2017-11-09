#pragma once

#include "MemoryHandler.h"

class PPU : public MemoryHandler
{
public:
	PPU();
	virtual ~PPU();

	virtual uint8_t ReadMem(uint16_t address) const = 0;
	virtual void WriteMem(uint16_t address, uint8_t value) = 0;
	virtual void Reset() = 0;
};
