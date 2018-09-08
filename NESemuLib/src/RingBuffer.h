#pragma once

#include "Assert.h"

#include <stdint.h>

template <class T, uint64_t size>
class RingBuffer
{
public:
    RingBuffer()
    {
        Reset();
    }
    
    void Write(T data)
    {
        _data[_writeIndex++] = data;
        if (_writeIndex + 1 >= size)
        {
            _writeIndex = 0;
        }

        if (_length < size)
        {
            ++_length;
        }

        if (_length == size)
        {
            IncReadIndex();
        }
    }
    
    T Read()
    {
        OMBAssert(_length > 0, "Buffer is empty!");

        const uint64_t currentReadIndex = _readIndex;
        IncReadIndex();
        --_length;

        return _data[currentReadIndex];
    }

    void Reset()
    {
        _writeIndex = 0;
        _readIndex = 0;
        _length = 0;
    }

    uint64_t GetLength() const { return _length; }
    bool IsFull() const { return _length == size; }

private:
    T _data[size];
    uint64_t _writeIndex;
    uint64_t _readIndex;
    uint64_t _length;

    void IncReadIndex()
    {
        ++_readIndex;
        if (_readIndex + 1 >= size)
        {
            _readIndex = 0;
        }
    }
};
