#include "IM004_MMC3.h"

#include "BitwiseUtils.h"

IM004_MMC3::IM004_MMC3(Hardware& hw) : 
    MemoryMapper(hw),
    _bankRegisterToUpdate(0)
{
}

uint8_t IM004_MMC3::ReadMem(uint16_t address)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Read);
    return handler.ReadMem(address);
}

void IM004_MMC3::WriteMem(uint16_t address, uint8_t value)
{
    const bool isEvenAddress = (address % 2) == 0;

    if (address >= 0x8000 && address <= 0x9FFE && isEvenAddress)
    {
        // Bank select ($8000-$9FFE, even)
        _bankRegisterToUpdate = BitwiseUtils::GetBitRange(value, 2, 3);
        _prgROMBankMode = BitwiseUtils::GetBitRange(value, 6, 1);
        _chrInversion = BitwiseUtils::GetBitRange(value, 7, 1);
    }
    else if (address >= 0x8001 && address <= 0x9FFF)
    {
        // Bank data ($8001-$9FFF, odd)
        OMBAssert(false, "TODO");
    }
    else if (address >= 0xA000 && address <= 0xBFFE && isEvenAddress)
    {
        // Mirroring ($A000-$BFFE, even)
        OMBAssert(false, "TODO");
    }
    else if (address >= 0xA001 && address <= 0xBFFF)
    {
        // PRG RAM protect ($A001-$BFFF, odd)
        OMBAssert(false, "TODO");
    }
    else if (address >= 0xC000 && address <= 0xDFFE && isEvenAddress)
    {
        // IRQ latch ($C000-$DFFE, even)
        OMBAssert(false, "TODO");
    }
    else if (address >= 0xC001 && address <= 0xDFFF)
    {
        // IRQ reload ($C001-$DFFF, odd)
        OMBAssert(false, "TODO");
    }
    else if (address >= 0xE000 && address <= 0xFFFE && isEvenAddress)
    {
        // IRQ disable ($E000-$FFFE, even)
    }
    else if (address >= 0xE001 && address <= 0xFFFF)
    {
        // IRQ enable ($E001-$FFFF, odd)
    }
    else
    {
        MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Write);
        handler.WriteMem(address, value);
    }
}

void IM004_MMC3::PowerOn()
{
    Reset();
}

void IM004_MMC3::Reset()
{
    //int _registers[8];
    _bankRegisterToUpdate = 0;
    _prgROMBankMode = 0;
    _chrInversion = 0;
}
