#pragma once

#include "Assert.h"

#include <stdint.h>

template <class T, uint64_t size>
class RingBuffer
{
public:
    RingBuffer()
    {
        Clear();
    }
    
    void Write(T data)
    {
        _data[_writeIndex++] = data;
        if (_writeIndex == size)
        {
            _writeIndex = 0;
        }

        if (_length == size)
        {
            IncReadIndex();
        }
        else
        {
            ++_length;
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

    void Clear()
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
        if (_readIndex == size)
        {
            _readIndex = 0;
        }
    }
};
