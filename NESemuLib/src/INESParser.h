#pragma once

#include "MirroringMode.h"

#include <stdint.h>

class PRGROM;
class MemoryMapper;

class INESParser
{
public:
    ~INESParser();
    void ParseHeader(const char* path);
    void ParseROMs(const char* path, MemoryMapper& memoryMapper, PRGROM& prgRom);

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