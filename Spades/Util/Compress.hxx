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
    /**
     * @brief Compress map data and save into chunks
     *
     * @param level Compression level
     * @param data Map data
     * @param length Length of map data
     * @param chunkSize Chunk size
     * @return Chunks or nullptr
     */
    static DataChunk* Compress(int level, void* data, uint32 length, uint32 chunkSize);
};

} // namespace Spades
