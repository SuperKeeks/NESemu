#include "NESemu.h"

#include "SizeOfArray.h"

#include "mappers/IM000_NROM.h"
#include "mappers/IM003_CNROM.h"
#include "mappers/IM004_MMC3.h"

NESemu::NESemu()
{
    _hw.PowerOn();
}

NESemu::~NESemu()
{
}

void NESemu::Load(const char* path)
{
    if (_mapper != nullptr)
    {
        delete _mapper;
    }

    _parser.ParseHeader(path);
    
    switch (_parser.GetMapperNumber())
    {
        case 0:
            _mapper = new IM000_NROM(_hw, _parser.GetPRGROMPageCount(), _parser.GetCHRROMPageCount());
            break;
        case 3:
            _mapper = new IM003_CNROM(_hw, _parser.GetPRGROMPageCount(), _parser.GetCHRROMPageCount());
            break;
        case 4:
            _mapper = new IM004_MMC3(_hw, _parser.GetPRGROMPageCount(), _parser.GetCHRROMPageCount());
            break;
        default:
            OMBAssert(false, "Unsupported memory mapper #%03d", _parser.GetMapperNumber());
            _mapper = nullptr;
    }
    _parser.ParseROMs(path, *_mapper);

    _parser.PrintInfo();
    _hw.sram.SetEnabled(_parser.IsSRAMEnabled());

    Reset();
}

void NESemu::Load(const uint8_t rom[], uint16_t romSize)
{
    OMBAssert(romSize == IM000_NROM::kMaxPRGROMSize || romSize == IM000_NROM::kMaxPRGROMSize / 2, "Unsupported ROM size");
    if (_mapper == nullptr)
    {
        _mapper = new IM000_NROM(_hw, romSize / MemoryMapper::kPRGROMPageSize, 1);
    }
    
    for (int i = 0; i < romSize; ++i)
    {
        _mapper->GetPGRROMPtr()[i] = rom[i];
    }

    Reset();
}

void NESemu::Load(const uint8_t rom[], uint16_t romSize, const uint8_t chrRom[], uint16_t chrRomSize)
{
    OMBAssert(chrRomSize == IM000_NROM::kMaxCHRROMSize, "Unsupported CHR-ROM size");
    _mapper = new IM000_NROM(_hw, romSize / MemoryMapper::kPRGROMPageSize, chrRomSize / MemoryMapper::kCHRROMPageSize);
    for (int i = 0; i < chrRomSize; ++i)
    {
        _mapper->GetCHRROMPtr()[i] = chrRom[i];
    }

    Load(rom, romSize);
}

void NESemu::Update(double delta, std::function<void()> lockAudio, std::function<void()> unlockAudio)
{
// Prevent big deltas while debugging (otherwise we might get stuck here for too long)
#if _DEBUG
    if (delta > 1.0f / 30)
    {
        delta = 1.0f / 30;
    }
#endif

    // PPU and CPU update
    const double masterDelta = kMasterClockSpeed * delta;
    const uint64_t ppuDelta = (uint64_t)std::round(masterDelta / 4); // The PPU runs 4x slower than the master clock
    for (int i = 0; i < ppuDelta; ++i)
    {
        if (i % 3 == 0)
        {
            // For every 3 PPU cycles, the CPU and APU run one
            _hw.cpu.Tick();
            _hw.apu.Tick(lockAudio, unlockAudio);
        }

        _hw.ppu.Tick();
    }
}

void NESemu::Reset()
{
    _hw.Reset(_mapper, _parser.GetMirroringMode());
}

void NESemu::SetControllerState(int controllerNumber, const Input::ControllerState& state)
{
    _hw.input.SetControllerState(controllerNumber, state);
}

uint8_t NESemu::ReadMem(uint16_t address)
{
    return _mapper->ReadMem(address);
}

void NESemu::WriteMem(uint16_t address, uint8_t value)
{
    _mapper->WriteMem(address, value);
}

HardwareStateSnapshot NESemu::GetSnapshot()
{
    HardwareStateSnapshot snapShot;

    snapShot.cpuState = _hw.cpu.GetSnapshot();
    snapShot.ramState = _hw.ram.GetSnapshot();
    snapShot.ppuState = _hw.ppu.GetSnapshot();

    return snapShot;
}

void NESemu::LoadSnapshot(HardwareStateSnapshot snapShot)
{
    _hw.cpu.LoadSnapshot(snapShot.cpuState);
    _hw.ram.LoadSnapshot(snapShot.ramState);
    _hw.ppu.LoadSnapshot(snapShot.ppuState);
}
