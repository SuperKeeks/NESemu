#include "INESParser.h"

#include "Assert.h"
#include "CHRROM.h"
#include "ROM.h"
#include "SizeOfArray.h"

#include <cstdlib>
#include <stdio.h>

INESParser::~INESParser()
{
}

void INESParser::Parse(const char* path, ROM& rom, CHRROM& chrRom)
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

    // PRG-ROM
    const size_t prgROMSize = 16384 * GetPRGROMPageCount();
    OMBAssert(prgROMSize <= ROM::kMaxROMSize, "Trying to load PRG-ROM bigger than it is supported");
    const size_t readPRGROMSize = fread_s(rom.GetROMPtr(), ROM::kMaxROMSize, sizeof(uint8_t), prgROMSize, file);
    OMBAssert(readPRGROMSize == prgROMSize, "Unexpected PRG-ROM size");
    rom.SetIs16KBROM(GetPRGROMPageCount() == 1);

    // CHR-ROM
    const size_t chrROMSize = CHRROM::kPageCHRROMSize * GetCHRROMPageCount();
    OMBAssert(chrROMSize <= CHRROM::kMaxCHRROMSize, "Trying to load CHR-ROM bigger than it is supported");
    const size_t readCHRROMSize = fread_s(chrRom.GetCHRROMPtr(), CHRROM::kMaxCHRROMSize, sizeof(uint8_t), chrROMSize, file);
    OMBAssert(readCHRROMSize == chrROMSize, "Unexpected CHR-ROM size");
    
    fclose(file);
}

void INESParser::PrintInfo() const
{
    printf("iNES ROM Info:\n");
    printf("==============\n");
    printf("PRG-ROM 16 KB page count: %d\n", GetPRGROMPageCount());
    printf("CHR-ROM 8 KB page count: %d\n", GetCHRROMPageCount());
    printf("Mirroring: %s\n", GetMirroringMode() == MirroringMode::Horizontal ? "Horizontal" : "Vertical");
    printf("SRAM: %s\n", IsSRAMEnabled() ? "Enabled" : "Disabled");
    printf("Trainer present: %s\n", IsTrainerPresent() ? "Yes" : "No");
    printf("Four-screen mirroring: %s\n", IsFourScreenMirroringEnabled() ? "Enabled" : "Disabled");
    printf("Mapper number: #%03d", GetMapperNumber());
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
