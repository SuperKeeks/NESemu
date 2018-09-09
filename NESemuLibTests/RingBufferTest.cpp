#include "CppUnitTest.h"

#include "RingBuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(RingBufferTest)
    {
    public:
        RingBuffer<int, 4> ringBuffer;

        TEST_METHOD(RingBuffer)
        {
            // 1 write, 1 read
            ringBuffer.Write(1);
            Assert::AreEqual((uint64_t)1, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(1, ringBuffer.Read());
            Assert::AreEqual((uint64_t)0, ringBuffer.GetLength());

            // Full write and read
            ringBuffer.Write(1);
            ringBuffer.Write(2);
            ringBuffer.Write(3);
            ringBuffer.Write(4);
            Assert::AreEqual((uint64_t)4, ringBuffer.GetLength());
            Assert::AreEqual(true, ringBuffer.IsFull());
            Assert::AreEqual(1, ringBuffer.Read());
            Assert::AreEqual((uint64_t)3, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(2, ringBuffer.Read());
            Assert::AreEqual((uint64_t)2, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(3, ringBuffer.Read());
            Assert::AreEqual((uint64_t)1, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(4, ringBuffer.Read());
            Assert::AreEqual((uint64_t)0, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());

            // Clear
            ringBuffer.Write(1);
            Assert::AreEqual((uint64_t)1, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            ringBuffer.Clear();
            Assert::AreEqual((uint64_t)0, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());

            // Overwrite 2 values
            ringBuffer.Write(1);
            Assert::AreEqual((uint64_t)1, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            ringBuffer.Write(2);
            Assert::AreEqual((uint64_t)2, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            ringBuffer.Write(3);
            Assert::AreEqual((uint64_t)3, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            ringBuffer.Write(4);
            Assert::AreEqual((uint64_t)4, ringBuffer.GetLength());
            Assert::AreEqual(true, ringBuffer.IsFull());
            ringBuffer.Write(5);
            Assert::AreEqual((uint64_t)4, ringBuffer.GetLength());
            Assert::AreEqual(true, ringBuffer.IsFull());
            ringBuffer.Write(6);
            Assert::AreEqual((uint64_t)4, ringBuffer.GetLength());
            Assert::AreEqual(true, ringBuffer.IsFull());
            Assert::AreEqual(3, ringBuffer.Read());
            Assert::AreEqual((uint64_t)3, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(4, ringBuffer.Read());
            Assert::AreEqual((uint64_t)2, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(5, ringBuffer.Read());
            Assert::AreEqual((uint64_t)1, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
            Assert::AreEqual(6, ringBuffer.Read());
            Assert::AreEqual((uint64_t)0, ringBuffer.GetLength());
            Assert::AreEqual(false, ringBuffer.IsFull());
        }

    };
}