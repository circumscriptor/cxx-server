/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/DataChunk.hxx"

typedef struct z_stream_s z_stream;

namespace Spades {

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
