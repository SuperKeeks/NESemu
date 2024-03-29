#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(LDXTest)
    {
    public:

        TEST_METHOD(LDX)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[MemoryMapper::kUnitTestPGRROMSize];
            rom[CPU::kResetVectorAddressL - MemoryMapper::kPGRROMStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - MemoryMapper::kPGRROMStartAddress] = MemoryMapper::kPGRROMStartAddress >> 8;
            int cycles;

            // Immediate
            int codeIndex = 0;
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x33; // #$33
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x00; // #0
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0xFF; // #-128

            // Zero Page
            rom[codeIndex++] = 0xA6; // LDX (Zero Page)
            rom[codeIndex++] = 0x44; // $44
            rom[codeIndex++] = 0xA6; // LDX (Zero Page)
            rom[codeIndex++] = 0x44; // $44
            rom[codeIndex++] = 0xA6; // LDX (Zero Page)
            rom[codeIndex++] = 0x44; // $44

            // Zero Page, Y
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x04; // #$4
            rom[codeIndex++] = 0xB6; // LDX (Zero Page, Y)
            rom[codeIndex++] = 0x40; // $40

            // Absolute
            rom[codeIndex++] = 0xAE; // LDX (Absolute)
            rom[codeIndex++] = 0x34; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            // Absolute, Y (don't cross boundary)
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x01; // #$01
            rom[codeIndex++] = 0xBE; // LDA (Absolute, Y)
            rom[codeIndex++] = 0x33; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            emu.Load(rom, MemoryMapper::kUnitTestPGRROMSize);

            ////////////
            // Immediate
            ////////////

            // Positive number
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            // Zero
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetX());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            // Negative number
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            ////////////
            // Zero Page
            ////////////

            // Positive number
            emu.WriteMem(0x44, 0x12);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x12, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            // Zero
            emu.WriteMem(0x44, 0x0);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetX());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            // Negative number
            emu.WriteMem(0x44, 0xFF);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            ///////////////
            // Zero Page, Y
            ///////////////
            emu.WriteMem(0x44, 0x66);
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x66, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            ///////////
            // Absolute
            ///////////
            emu.WriteMem(0x1234, 0x77);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x77, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            //////////////
            // Absolute, Y
            //////////////
            emu.WriteMem(0x1234, 0x22);
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x22, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);
        }

    };
}