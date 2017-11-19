#pragma once

#include "MemoryHandler.h"

class ROM : public MemoryHandler
{
public:
	static const uint16_t kMaxROMSize = 32768;
	static const uint16_t kStartAddress = 0x8000;

	ROM();
	virtual ~ROM();

	virtual uint8_t ReadMem(uint16_t address);
	virtual void WriteMem(uint16_t address, uint8_t value);
	virtual void PowerOn();
	virtual void Reset();

	// For ROM parsing only
	uint8_t* GetROMPtr() { return _rom; }
	void SetIs16KBROM(bool is16KROM) { _is16KBROM = is16KROM; }

private:
	uint8_t _rom[kMaxROMSize];
	bool _is16KBROM;

	uint16_t ConvertToIndex(uint16_t address) const;
};
