#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(PHAPLATest)
    {
    public:

        TEST_METHOD(PHAPLA)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[MemoryMapper::kUnitTestPGRROMSize];
            rom[CPU::kResetVectorAddressL - MemoryMapper::kPGRROMStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - MemoryMapper::kPGRROMStartAddress] = MemoryMapper::kPGRROMStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x48; // PHA
            rom[codeIndex++] = 0x68; // PLA

            emu.Load(rom, MemoryMapper::kUnitTestPGRROMSize);

            cpu.SetAccumulator(0x48);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x48, cpu.PeekStack(0));
            Assert::AreEqual(3, cycles);

            cpu.SetAccumulator(0xFF);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual((uint8_t)0x48, cpu.GetAccumulator());
            Assert::AreEqual(4, cycles);
        }

    };
}