#include "CppUnitTest.h"

#include "BitwiseUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(BITTest)
    {
    public:

        TEST_METHOD(GetBitRange)
        {
            const uint8_t ff = 0xFF;
            
            const uint8_t allBits = BitwiseUtils::GetBitRange(ff, 7, 8);
            Assert::AreEqual(ff, allBits);

            uint8_t result = BitwiseUtils::GetBitRange(ff, 0, 1);
            Assert::AreEqual((uint8_t)1, result);

            result = BitwiseUtils::GetBitRange(ff, 7, 1);
            Assert::AreEqual((uint8_t)1, result);

            result = BitwiseUtils::GetBitRange(0x1, 7, 1);
            Assert::AreEqual((uint8_t)0, result);

            result = BitwiseUtils::GetBitRange(0x1, 0, 1);
            Assert::AreEqual((uint8_t)1, result);

            result = BitwiseUtils::GetBitRange(0xCC, 7, 4);
            Assert::AreEqual((uint8_t)0xC, result);
        }

    };
}