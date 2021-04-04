/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Types.hxx"

namespace Spades {

struct DataChunk
{
    uint8*     mChunk;
    uint32     mLength;
    uint32     mSize;
    DataChunk* mNext;

    DataChunk(uint32 size) : mSize{size}, mLength{0}, mNext{nullptr}
    {
        mChunk = new uint8[size];
    }

    ~DataChunk()
    {
        delete[] mChunk;
    }

    DataChunk* Push(uint32 size)
    {
        mNext = new DataChunk(size);
        return mNext;
    }

    DataChunk* Pop()
    {
        DataChunk* next = mNext;
        delete this;
        return next;
    }
};

} // namespace Spades
