#pragma once

#include "Assert.h"

#include <stdint.h>

class MemoryHandler
{
public:
	virtual ~MemoryHandler() {}
	virtual uint8_t ReadMem(uint16_t address) { OMBAssert(false, "Don't use the MemoryHandler class directly!"); return 0; }
	virtual void WriteMem(uint16_t address, uint8_t value) {}
	virtual void Reset() {}
};