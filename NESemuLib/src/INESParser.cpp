#include "INESParser.h"

#include "Assert.h"
#include "LogUtils.h"
#include "MemoryMapper.h"
#include "SizeOfArray.h"

#include <cstdlib>
#include <stdio.h>

INESParser::~INESParser()
{
}

void INESParser::ParseHeader(const char* path)
{
    FILE* file;
    fopen_s(&file, path, "rb");

    // Header
    const size_t headerSize = fread_s(_header, sizeof(_header), sizeof(uint8_t), sizeofarray(_header), file);
    OMBAssert(headerSize == 16, "Unexpected header size");
    OMBAssert(_header[0] == 'N', "Unexpected character");
    OMBAssert(_header[1] == 'E', "Unexpected character");
    OMBAssert(_header[2] == 'S', "Unexpected character");
    OMBAssert(_header[3] == 0x1A, "Unexpected character");
    for (int i = 8; i < 16; ++i)
    {
        OMBAssert(_header[i] == 0, "Unsupported ROM format");
    }

    fclose(file);
}

void INESParser::ParseROMs(const char* path, MemoryMapper& memoryMapper)
{
    FILE* file;
    fopen_s(&file, path, "rb");
    fseek(file, sizeof(uint8_t) * sizeofarray(_header), 0);

    // PRG-ROM
    const size_t prgROMSize = MemoryMapper::kPRGROMPageSize * GetPRGROMPageCount();
    OMBAssert(prgROMSize <= memoryMapper.GetPGRROMMaxSize(), "Trying to load PRG-ROM bigger than it is supported");
    const size_t readPRGROMSize = fread_s(memoryMapper.GetPGRROMPtr(), memoryMapper.GetPGRROMMaxSize(), sizeof(uint8_t), prgROMSize, file);
    OMBAssert(readPRGROMSize == prgROMSize, "Unexpected PRG-ROM size");
    memoryMapper.SetPGRROMPageCount(GetPRGROMPageCount());

    // CHR-ROM
    const size_t chrROMSize = MemoryMapper::kCHRROMPageSize * GetCHRROMPageCount();
    OMBAssert(chrROMSize <= memoryMapper.GetCHRROMMaxSize(), "Trying to load CHR-ROM bigger than it is supported");
    const size_t readCHRROMSize = fread_s(memoryMapper.GetCHRROMPtr(), memoryMapper.GetCHRROMMaxSize(), sizeof(uint8_t), chrROMSize, file);
    OMBAssert(readCHRROMSize == chrROMSize, "Unexpected CHR-ROM size");
    
    fclose(file);
}

void INESParser::PrintInfo() const
{
    Log::Info("iNES ROM Info:");
    Log::Info("==============");
    Log::Info("PRG-ROM 16 KB page count: %d", GetPRGROMPageCount());
    Log::Info("CHR-ROM 8 KB page count: %d", GetCHRROMPageCount());
    Log::Info("Mirroring: %s", GetMirroringMode() == MirroringMode::Horizontal ? "Horizontal" : "Vertical");
    Log::Info("SRAM: %s", IsSRAMEnabled() ? "Enabled" : "Disabled");
    Log::Info("Trainer present: %s", IsTrainerPresent() ? "Yes" : "No");
    Log::Info("Four-screen mirroring: %s", IsFourScreenMirroringEnabled() ? "Enabled" : "Disabled");
    Log::Info("Mapper number: #%03d\n", GetMapperNumber());
}

uint8_t INESParser::GetPRGROMPageCount() const
{
    return _header[4];
}

uint8_t INESParser::GetCHRROMPageCount() const
{
    return _header[5];
}

MirroringMode INESParser::GetMirroringMode() const
{
    if ((_header[6] & 1 << 0) == 1)
    {
        return MirroringMode::Vertical;
    }
    else
    {
        return MirroringMode::Horizontal;
    }
}

bool INESParser::IsSRAMEnabled() const
{
    if ((_header[6] & (1 << 1)) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool INESParser::IsTrainerPresent() const
{
    if ((_header[6] & (1 << 2)) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool INESParser::IsFourScreenMirroringEnabled() const
{
    if ((_header[6] & (1 << 3)) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int INESParser::GetMapperNumber() const
{
    return _header[7] + (_header[6] >> 4);
}
