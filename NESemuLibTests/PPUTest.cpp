#include "CppUnitTest.h"

#include "NESemu.h"
#include "SizeOfArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(PPUTest)
    {
    public:

        TEST_METHOD(PPUPatternTable)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            uint8_t chrRom[CHRROM::kMaxCHRROMSize];
            for (int i = 0; i < sizeofarray(chrRom); ++i)
            {
                chrRom[i] = i % 256;
            }

            emu.Load(rom, ROM::kMaxROMSize, chrRom, CHRROM::kMaxCHRROMSize);

            // Verify PatternTable in VRAM and its shadows
            for (int i = 0; i < 4; ++i)
            {
                // Read 0x2002 to reset PPUAddr status
                emu.ReadMem(PPU::kPPUStatusAddress);
                
                // Write address to read from
                emu.WriteMem(PPU::kPPUAddrAddress, 0x40 * i);
                emu.WriteMem(PPU::kPPUAddrAddress, 0x00);

                // Discard first read, as it has a cached value
                emu.ReadMem(PPU::kPPUDataAddress);

                // Verify all values
                for (int i = 0; i < sizeofarray(chrRom); ++i)
                {
                    Assert::AreEqual(i % 256, (int)emu.ReadMem(PPU::kPPUDataAddress));
                }
            }
        }

        TEST_METHOD(PPUNameTable)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];

            emu.Load(rom, ROM::kMaxROMSize);

            // Read 0x2002 to reset PPUAddr status
            emu.ReadMem(PPU::kPPUStatusAddress);

            emu.GetPPU()->SetMirroringMode(MirroringMode::Vertical);

            // Set address of Nametable 0
            emu.WriteMem(PPU::kPPUAddrAddress, 0x20);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);

            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                // Write values to Nametable 0-2
                emu.WriteMem(PPU::kPPUDataAddress, 0x33);
            }

            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                // Write values to Nametable 1-3
                emu.WriteMem(PPU::kPPUDataAddress, 0x66);
            }

            // Check Nametable 0
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x20);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 0 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x30);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x24);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x34);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x28);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x38);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x2C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3 (Shadow, partial)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x3C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < 0x2FF; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            /////////////////////////////////////
            // SAME BUT WITH HORIZONTAL MIRRORING
            /////////////////////////////////////
            emu.GetPPU()->SetMirroringMode(MirroringMode::Horizontal);

            // Check Nametable 0
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x20);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 0 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x30);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x24);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x34);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x28);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x38);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x2C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3 (Shadow, partial)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x2C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < 0x2FF; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }
        }

    };
}