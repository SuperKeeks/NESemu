#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(LSRTest)
    {
    public:

        TEST_METHOD(LSR)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // LSR Accumulator, change to zero, carry
            rom[codeIndex++] = 0x4A; // ASL Accumulator

            // LSR Zero Page, change to positive, no carry
            rom[codeIndex++] = 0x46; // ASL Zero Page
            rom[codeIndex++] = 0x33; // Zero page value

            emu.Load(rom, ROM::kMaxROMSize);

            cpu.SetAccumulator(0x01);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0x00, cpu.GetAccumulator());
            Assert::AreEqual(2, cycles);

            emu.WriteMem(0x33, 0x80);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0x40, cpu.GetAccumulator());
            Assert::AreEqual(5, cycles);
        }

    };
}