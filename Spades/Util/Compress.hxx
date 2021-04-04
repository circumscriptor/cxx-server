/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"

typedef struct z_stream_s z_stream;

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

class Compressor
{
  public:
    Compressor(int level);
    ~Compressor();

    DataChunk* Compress(void* data, uint32 length, uint32 chunkSize);

    bool IsValid() const noexcept
    {
        return mIsValid;
    }

  private:
    z_stream* stream;
    bool      mIsValid{true};
};

} // namespace Spades
