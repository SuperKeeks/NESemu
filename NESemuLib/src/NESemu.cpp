#include "NESemu.h"

#include "SizeOfArray.h"

NESemu::NESemu()
{
    _cpu.PowerOn();
    _ram.PowerOn();
    _ppu.PowerOn();
    _apu.PowerOn();
    _input.PowerOn();
    _mm5.PowerOn();
    _sram.PowerOn();
    _rom.PowerOn();
    _chrRom.PowerOn();
}

NESemu::~NESemu()
{
}

void NESemu::Load(const char* path)
{
    _parser.Parse(path, _rom, _chrRom);
    _parser.PrintInfo();
    _sram.SetEnabled(_parser.IsSRAMEnabled());

    Reset();
}

void NESemu::Load(const uint8_t rom[], uint16_t romSize)
{
    OMBAssert(romSize == ROM::kMaxROMSize || romSize == ROM::kMaxROMSize / 2, "Unsupported ROM size");
    for (int i = 0; i < romSize; ++i)
    {
        _rom.GetROMPtr()[i] = rom[i];
    }
    _rom.SetIs16KBROM(romSize == ROM::kMaxROMSize / 2);

    Reset();
}

void NESemu::Load(const uint8_t rom[], uint16_t romSize, const uint8_t chrRom[], uint16_t chrRomSize)
{
    OMBAssert(chrRomSize == CHRROM::kMaxCHRROMSize, "Unsupported CHR-ROM size");
    for (int i = 0; i < chrRomSize; ++i)
    {
        _chrRom.GetCHRROMPtr()[i] = chrRom[i];
    }

    Load(rom, romSize);
}

void NESemu::Update()
{
    _cpu.ExecuteNextInstruction();
}

uint8_t NESemu::ReadMem(uint16_t address)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address);
    return handler.ReadMem(address);
}

void NESemu::WriteMem(uint16_t address, uint8_t value)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address);
    handler.WriteMem(address, value);
}

void NESemu::Reset()
{
    _cpu.Reset(this);
    _ram.Reset();
    _ppu.Reset(this, &_chrRom, _parser.GetMirroringMode());
    _apu.Reset();
    _input.Reset();
    _mm5.Reset();
    _sram.Reset();
    _rom.Reset();
    _chrRom.Reset();
}

MemoryHandler& NESemu::GetMemoryHandlerForAddress(uint16_t address)
{
    if (address >= 0x0 && address < 0x2000)
    {
        return _ram;
    }
    else if ((address >= 0x2000 && address < 0x4000) || address == 0x4014)
    {
        return _ppu;
    }
    else if ((address >= 0x4000 && address < 0x4014) || address == 0x4015)
    {
        return _apu;
    }
    else if (address == 0x4016 || address == 0x4017)
    {
        return _input;
    }
    else if (address >= 0x5000 && address < 0x6000)
    {
        return _mm5;
    }
    else if (address >= 0x6000 && address < 0x8000)
    {
        return _sram;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        return _rom;
    }
    else
    {
        OMBAssert(false, "Unimplemented!");
        return *(new MemoryHandler());
    }
}
