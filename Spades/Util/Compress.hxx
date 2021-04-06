/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/DataChunk.hxx"

namespace Spades {

/**
 * @brief Compressor, used by protocol to compress map data
 *
 */
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
    static auto Compress(int level, void* data, uint32 length, uint32 chunkSize) -> DataChunk*;
};

} // namespace Spades
