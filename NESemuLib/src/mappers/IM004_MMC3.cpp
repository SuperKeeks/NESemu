#include "IM004_MMC3.h"

#include "BitwiseUtils.h"

IM004_MMC3::IM004_MMC3(Hardware& hw, size_t pgrPageCount, size_t chrPageCount) :
    MemoryMapper(hw, pgrPageCount, chrPageCount),
    _bankRegisterToUpdate(0)
{
}

uint8_t IM004_MMC3::ReadCHRROMMem(uint16_t address)
{
    if (address <= 0x03FF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(2, address);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(0, address);
        }
    }
    else if (address <= 0x07FF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(3, address - 0x0400);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(0, address);
        }
    }
    else if (address <= 0x0BFF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(4, address - 0x0800);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(1, address - 0x0800);
        }
    }
    else if (address <= 0x0FFF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(5, address - 0x0C00);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(1, address - 0x0800);
        }
    }
    else if (address <= 0x13FF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(0, address - 0x1000);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(2, address - 0x1000);
        }
    }
    else if (address <= 0x17FF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(0, address - 0x1000);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(3, address - 0x1400);
        }
    }
    else if (address <= 0x1BFF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(1, address - 0x1800);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(4, address - 0x1800);
        }
    }
    else if (address <= 0x1FFF)
    {
        if (_chrInversion)
        {
            return GetCHRROMDataBasedOnRegister(1, address - 0x1800);
        }
        else
        {
            return GetCHRROMDataBasedOnRegister(5, address - 0x1C00);
        }
    }
    else
    {
        OMBAssert(false, "Address is out of CHRROM bounds!");
        return 0;
    }
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
        // R6 and R7 will ignore the top two bits, as the MMC3 has only 6 PRG ROM address lines
        if (_bankRegisterToUpdate >= 6)
        {
            value = BitwiseUtils::GetBitRange(value, 5, 6);
        }

        // Bank data ($8001-$9FFF, odd)
        _registers[_bankRegisterToUpdate] = value;
    }
    else if (address >= 0xA000 && address <= 0xBFFE && isEvenAddress)
    {
        // Mirroring ($A000-$BFFE, even)
        const bool isHorizontalMirroring = BitwiseUtils::GetBitRange(value, 0, 1);
        _hw.ppu.SetMirroringMode(isHorizontalMirroring ? MirroringMode::Horizontal : MirroringMode::Vertical);
    }
    else if (address >= 0xA001 && address <= 0xBFFF)
    {
        // PRG RAM protect ($A001-$BFFF, odd)
        const bool allowPRGRAMWrites = BitwiseUtils::GetBitRange(value, 6, 1) == 0;
        const bool enablePRGRAM = BitwiseUtils::GetBitRange(value, 7, 1) == 1;
        _hw.sram.SetEnabled(enablePRGRAM);
        _hw.sram.SetAllowWrite(allowPRGRAMWrites);
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
        _enableIRQ = false;
        // TODO: Acknowledge any pending interrupts
    }
    else if (address >= 0xE001 && address <= 0xFFFF)
    {
        // IRQ enable ($E001-$FFFF, odd)
        _enableIRQ = true;
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
    _bankRegisterToUpdate = 0;
    _prgROMBankMode = 0;
    _chrInversion = 0;
}

uint8_t IM004_MMC3::ReadPRGROMMem(uint16_t address)
{
    if (address < kPGRROMStartAddress)
    {
        OMBAssert(false, "Invalid PRG ROM address!");
        return 0;
    }

    if (address <= 0x9FFF)
    {
        if (_prgROMBankMode == 0)
        {
            return _pgrROM[(_registers[6] * kPGRROMBankSize) + (address - kPGRROMStartAddress)];
        }
        else
        {
            const size_t secondLast8KBPageIndex = (_pgrROMPageCount * 2) - 2;
            const size_t index = (secondLast8KBPageIndex * kPGRROMBankSize) + (address - kPGRROMStartAddress);
            return _pgrROM[index];
        }
    }
    else if (address <= 0xBFFF)
    {
        return _pgrROM[(_registers[7] * kPGRROMBankSize) + (address - 0xA000)];
    }
    else if (address <= 0xDFFF)
    {
        if (_prgROMBankMode == 0)
        {
            const size_t secondLast8KBPageIndex = (_pgrROMPageCount * 2) - 2;
            const size_t index = (secondLast8KBPageIndex * kPGRROMBankSize) + (address - 0xC000);
            return _pgrROM[index];
        }
        else
        {
            return _pgrROM[(_registers[6] * kPGRROMBankSize) + (address - 0xC000)];
        }
    }
    else
    {
        const size_t last8KBPageIndex = (_pgrROMPageCount * 2) - 1;
        const size_t index = (last8KBPageIndex * kPGRROMBankSize) + (address - 0xE000);
        return _pgrROM[index];
    }
}

uint8_t IM004_MMC3::GetCHRROMDataBasedOnRegister(int registerIndex, uint16_t address)
{
    int registerValue = _registers[registerIndex];
    if ((registerIndex == 0 || registerIndex == 1) && registerValue % 2 != 0)
    {
        // "2KB banks may only select even numbered CHR banks. (The lowest bit is ignored.)"
        registerValue = registerValue & ~(0x1);
    }

    return _chrROM[(registerValue * kCHRROMBankSize) + address];
}
