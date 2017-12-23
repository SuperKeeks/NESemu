#include "CppUnitTest.h"

#include "NESemu.h"
#include "SizeOfArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(PPUTest)
    {
    public:

        TEST_METHOD(PatternTable)
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
                emu.ReadMem(0x2002);
                
                // Write address to read from
                emu.WriteMem(0x2006, 0x40 * i);
                emu.WriteMem(0x2006, 0x00);

                // Discard first read, as it has a cached value
                emu.ReadMem(0x2007);

                // Verify all values
                for (int i = 0; i < sizeofarray(chrRom); ++i)
                {
                    Assert::AreEqual(i % 256, (int)emu.ReadMem(0x2007));
                }
            }
        }

    };
}