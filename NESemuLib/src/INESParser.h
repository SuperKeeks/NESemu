#pragma once

#include <stdint.h>

class ROM;

class INESParser
{
public:
	enum MirroringMode
	{
		Horizontal,
		Vertical
	};

	~INESParser();
	void Parse(const char* path, ROM& rom);

	void PrintInfo() const;
	uint8_t GetPRGROMPageCount() const;
	uint8_t GetCHRROMPageCount() const;
	MirroringMode GetMirroringMode() const;
	bool IsSRAMEnabled() const;
	bool IsTrainerPresent() const;
	bool IsFourScreenMirroringEnabled() const;
	int GetMapperNumber() const;

private:
	bool _loaded = false;
	uint8_t _header[16];
};