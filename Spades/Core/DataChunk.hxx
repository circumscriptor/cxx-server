/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Types.hxx"

namespace Spades {

/**
 * @brief Map chunk
 *
 */
struct DataChunk
{
    uint8*     mChunk;  //!< Pointer to the memory block
    uint32     mLength; //!< Length of the contents of the memory block
    uint32     mSize;   //!< Size of the memory block
    DataChunk* mNext;   //!< Next data chunk

    /**
     * @brief Construct a new DataChunk object
     *
     * @param size Size of the memory block
     */
    DataChunk(uint32 size) : mSize{size}, mLength{0}, mNext{nullptr}
    {
        mChunk = new uint8[size];
    }

    /**
     * @brief Destroy the DataChunk object
     *
     */
    ~DataChunk()
    {
        delete[] mChunk;
    }

    /**
     * @brief Push and return new chunk
     *
     * @param size Size of the memory block
     * @return Next memory chunk
     */
    auto Push(uint32 size) -> DataChunk*
    {
        mNext = new DataChunk(size);
        return mNext;
    }

    /**
     * @brief Destroy this chunk and return next
     *
     * @return Next chunk or nullptr
     */
    auto Pop() -> DataChunk*
    {
        DataChunk* next = mNext;
        delete this;
        return next;
    }
};

} // namespace Spades
