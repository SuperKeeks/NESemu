#pragma once

#include <stdint.h>

class NESemu
{
public:
	NESemu();
	~NESemu();

private:
	uint8_t ReadMem(uint16_t address);
	void WriteMem(uint16_t address, uint8_t value);
};
