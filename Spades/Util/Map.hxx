/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"

#include <vector>

namespace Spades {

class Map
{
    friend class Convert;

  public:
    Map()
    {
        mBlocks = new uint32[1 << 19];
        mColors = new uint32[1 << 24];
    }

    ~Map()
    {
        delete[] mBlocks;
        delete[] mColors;
    }

    static constexpr uint32 GetOffset(uint32 x, uint32 y, uint32 z) noexcept
    {
        return (((x << 9) + y) << 6) + z;
    }

    bool IsBlock(uint32 offset) const
    {
        return mBlocks[offset >> 5] >> (offset & 0x1F);
    }

    bool IsBlock(uint32 x, uint32 y, uint32 z) const
    {
        return IsBlock(GetOffset(x, y, z));
    }

    void SetBlock(uint32 offset, bool value)
    {
        if (value) {
            mBlocks[offset >> 5] |= (1 << (offset & 0x1F));
        } else {
            mBlocks[offset >> 5] &= ~(1 << (offset & 0x1F));
        }
    }

    void SetBlock(uint32 x, uint32 y, uint32 z, bool value)
    {
        SetBlock(GetOffset(x, y, z), value);
    }

    uint32 GetColor(uint32 x, uint32 y, uint32 z) const
    {
        return mColors[GetOffset(x, y, z)];
    }

    uint32 GetColor(uint32 offset) const
    {
        return mColors[offset];
    }

    void SetColor(uint32 offset, uint32 value)
    {
        mColors[offset] = value;
    }

    void SetColor(uint32 x, uint32 y, uint32 z, uint32 value)
    {
        SetColor(GetOffset(x, y, z), value);
    }

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

    bool Load(uint8* data, uint32 length);

    void Save(std::vector<uint8>& output);

  private:
    uint32* mBlocks;
    uint32* mColors;
};

} // namespace Spades
