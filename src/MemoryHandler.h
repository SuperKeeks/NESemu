#pragma once

#include <stdint.h>

class MemoryHandler
{
public:
	virtual ~MemoryHandler() {}
	virtual uint8_t ReadMem(uint16_t address) const = 0;
	virtual void WriteMem(uint16_t address, uint8_t value) = 0;
};