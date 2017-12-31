#include "CppUnitTest.h"

#include "Input.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(InputTest)
    {
    public:

        TEST_METHOD(Controller1Test)
        {
            Input input;
            input.Reset();

            Input::ControllerState controller1State;
            controller1State.A = true;
            controller1State.Right = true;
            input.SetControllerState(1, controller1State);

            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x0);
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // A
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // B
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Select
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Start
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Up
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Down
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Left
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // Right

            // Strobe again
            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x0);
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // A
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // B
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Select
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Start
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Up
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Down
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Left
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // Right
            for (int i = 0; i < 8; ++i)
            {
                input.ReadMem(Input::kController1Address);
            }
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Signature #1
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Signature #2
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Signature #3
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // Signature #4
        }

        TEST_METHOD(Controller2Test)
        {
            Input input;
            input.Reset();

            Input::ControllerState controller2State;
            controller2State.B = true;
            controller2State.Select = true;
            controller2State.Start = true;
            controller2State.Up = true;
            controller2State.Down = true;
            controller2State.Left = true;
            input.SetControllerState(2, controller2State);

            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x0);
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // A
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // B
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Select
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Start
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Up
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Down
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Left
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Right
            for (int i = 0; i < 8; ++i)
            {
                input.ReadMem(Input::kController2Address);
            }
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Signature #1
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Signature #2
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Signature #3
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Signature #4
        }

        TEST_METHOD(Controller3Test)
        {
            Input input;
            input.Reset();

            Input::ControllerState controllerState;
            controllerState.Start = true;
            controllerState.Down = true;
            input.SetControllerState(3, controllerState);

            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x0);
            for (int i = 0; i < 8; ++i)
            {
                // Skip Controller 1 info
                input.ReadMem(Input::kController1Address);
            }
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // A
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // B
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Select
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // Start
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Up
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x1); // Down
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Left
            Assert::AreEqual((int)input.ReadMem(Input::kController1Address), 0x0); // Right
        }

        TEST_METHOD(Controller4Test)
        {
            Input input;
            input.Reset();

            Input::ControllerState controllerState;
            controllerState.B = true;
            controllerState.Select = true;
            input.SetControllerState(4, controllerState);

            input.WriteMem(Input::kController1Address, 0x1);
            input.WriteMem(Input::kController1Address, 0x0);
            for (int i = 0; i < 8; ++i)
            {
                // Skip Controller 2 info
                input.ReadMem(Input::kController2Address);
            }
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // A
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // B
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x1); // Select
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Start
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Up
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Down
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Left
            Assert::AreEqual((int)input.ReadMem(Input::kController2Address), 0x0); // Right
        }

    };
}