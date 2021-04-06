/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"

#include <vector>

namespace Spades {

/**
 * @brief Map, 512x512x64 blocks
 *
 */
class Map
{
  public:
    /**
     * @brief Construct a new Map object
     *
     */
    Map()
    {
        mBlocks = new uint32[1 << 19];
        mColors = new uint32[1 << 24];
    }

    /**
     * @brief Destroy the Map object
     *
     */
    ~Map()
    {
        delete[] mBlocks;
        delete[] mColors;
    }

    /**
     * @brief Calculate the offset of the block from the coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return The offset of the block
     */
    static constexpr uint32 GetOffset(uint32 x, uint32 y, uint32 z) noexcept
    {
        return (((x << 9) + y) << 6) + z;
    }

    /**
     * @brief Calculate the offset of the block from the coordinates (32 blocks per offset)
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return The offset of the block (32 blocks per offset)
     */
    static constexpr uint32 GetOffset32(uint32 x, uint32 y, uint32 z) noexcept
    {
        return (((x << 9) + y) << 1) + (z >> 5);
    }

    /**
     * @brief Check whether the block at the given index is solid
     *
     * @param offset The offset
     * @return true If the block is solid
     * @return false If the block is not solid, i.e. is air
     */
    bool IsBlock(uint32 offset) const
    {
        return (mBlocks[offset >> 5] & (1 << (offset & 0x1F))) != 0;
    }

    /**
     * @brief Check whether the block at the given coordinates is solid
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return true If the block is solid
     * @return false If the block is not solid, i.e. is air
     */
    bool IsBlock(uint32 x, uint32 y, uint32 z) const
    {
        return (mBlocks[GetOffset32(x, y, z)] & (1 << (z & 0x1F))) != 0;
    }

    /**
     * @brief Set the state of the block on the given offset
     *
     * @param offset The offset
     * @param value The new state of the block
     */
    void SetBlock(uint32 offset, bool value)
    {
        if (value) {
            mBlocks[offset >> 5] |= (1 << (offset & 0x1F));
        } else {
            mBlocks[offset >> 5] &= ~(1 << (offset & 0x1F));
        }
    }

    /**
     * @brief Set the state of the block on the given coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @param value The new state of the block
     */
    void SetBlock(uint32 x, uint32 y, uint32 z, bool value)
    {
        if (value) {
            mBlocks[GetOffset32(x, y, z)] |= (1 << (z & 0x1F));
        } else {
            mBlocks[GetOffset32(x, y, z)] &= ~(1 << (z & 0x1F));
        }
    }

    /**
     * @brief Get the color of the block on the given offset
     *
     * @param offset The offset
     * @return Block color in ARGB format
     */
    uint32 GetColor(uint32 offset) const
    {
        return mColors[offset];
    }

    /**
     * @brief Get the color of the block on the given coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return Block color in ARGB format
     */
    uint32 GetColor(uint32 x, uint32 y, uint32 z) const
    {
        return mColors[GetOffset(x, y, z)];
    }

    /**
     * @brief Set the color of the block on the given offset
     *
     * @param offset The offset
     * @param value Block color in ARGB format
     */
    void SetColor(uint32 offset, uint32 value)
    {
        mColors[offset] = value;
    }

    /**
     * @brief Set the color of the block on the given coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @param value Block color in ARGB format
     */
    void SetColor(uint32 x, uint32 y, uint32 z, uint32 value)
    {
        mColors[GetOffset(x, y, z)] = value;
    }

    /**
     * @brief Check whether the block is a surface block (is visible from at least one side)
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return true If the block is a surface block
     * @return false If the block is not solid or if is not a surface block
     */
    bool IsSurface(uint32 x, uint32 y, uint32 z) const
    {
        if (!IsBlock(x, y, z)) {
            return false;
        }
        if (x > 0 && !IsBlock(x - 1, y, z)) { // check block on left
            return true;
        }
        if (x + 1 < 512 && !IsBlock(x + 1, y, z)) { // check block on right
            return true;
        }
        if (y > 0 && !IsBlock(x, y - 1, z)) { // check block up
            return true;
        }
        if (y + 1 < 512 && !IsBlock(x, y + 1, z)) { // check block down
            return true;
        }
        if (z > 0 && !IsBlock(x, y, z - 1)) { // check block above
            return true;
        }
        if (z + 1 < 64 && !IsBlock(x, y, z + 1)) { // check block below
            return true;
        }
        return false;
    }

    /**
     * @brief Get the height at which the highest-placed block is located
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @return The z-coordinate of the block
     */
    uint8 GetHeight(uint32 x, uint32 y) const
    {
        // uint32 offset = (((x << 9) + y) << 6) >> 5;
        uint32 offset = (((x << 9) + y) << 1);
        uint32 check  = mBlocks[offset];
        uint8  z      = 0;
        if (check == 0) {
            z += 32;
            check = mBlocks[offset + 1];
            if (check == 0) {
                return z + 32;
            }
        }
        for (; (check >>= 1) != 0; ++z) {
        }
        return z;
    }

    /**
     * @brief Load map from data (VXL format)
     *
     * @param data Data (VXL format)
     * @param length The length of the data
     * @return true On success
     * @return false On failure
     */
    bool Load(uint8* data, uint32 length);

    /**
     * @brief Store map in the output buffer (VXL format)
     *
     * @param output The ouput buffer (VXL format)
     */
    void Save(std::vector<uint8>& output);

  private:
    uint32* mBlocks; //!< Array/bitset of block states (32 blocks each)
    uint32* mColors; //!< Array of colors
};

} // namespace Spades
