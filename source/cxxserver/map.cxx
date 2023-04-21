///
/// @file map.cxx
/// @brief Map
///
/// @copyright Copyright (c) 2023
///
///

#include "map.hxx"

#include <cmath>

namespace cxxserver {

void Map::modifyBlock(std::uint32_t x, std::uint32_t y, std::uint32_t z, bool value, std::uint32_t color)
{
    if (z > 61)
    {
        return; // bottom blocks cannot be modified
    }
    auto offset = Map::offset(x, y, z);
    assert(offset < SIZE_XYZ);
    setSolid(offset, value);
    setColor(offset, color);
    mChanged = true;
}

void Map::destroyBlock(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
    if (z > 61)
    {
        return; // bottom blocks cannot be broken
    }
    setSolid(x, y, z, false);
    mChanged = true;
}

void Map::destroyBlockSecondary(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
    if (z > 61)
    {
        return; // bottom blocks cannot be broken
    }
    auto offset = Map::offset(x, y, z);
    if (z > 0)
    {
        setSolid(offset - 1, false);
    }
    if (z < 61)
    {
        setSolid(offset + 1, false);
    }
    setSolid(offset, false);
    mChanged = true;
}

void Map::destroyBlockGrenade(std::int32_t x, std::int32_t y, std::int32_t z)
{
    for (int dz = -1; dz < 2; ++dz)
    {
        auto _z = z + dz;
        if (_z < 0 || _z > 61)
        {
            continue;
        }
        for (int dy = -1; dy < 2; ++dy)
        {
            auto _y = y + dy;
            if (_y < 0 || y >= 512)
            {
                continue;
            }
            for (int dx = -1; dx < 2; ++dx)
            {
                auto _x = x + dx;
                if (_x < 0 || x >= 512)
                {
                    continue;
                }
                setSolid(static_cast<std::uint32_t>(_x), static_cast<std::uint32_t>(_y), static_cast<std::uint32_t>(_z), false);
                mChanged = true;
            }
        }
    }
}

void Map::setSolid(std::uint32_t offset, bool value)
{
    assert(offset < SIZE_XYZ);
    if (value)
    {
        mBlocks.at(offset).data |= 0x01;
    }
    else
    {
        mBlocks.at(offset).data &= ~0x01;
    }
}

bool Map::isSolid(std::uint32_t offset) const
{
    assert(offset < SIZE_XYZ);
    return mBlocks.at(offset).data & 0x01;
}

void Map::setSolid(std::uint32_t x, std::uint32_t y, std::uint32_t z, bool value)
{
    setSolid(offset(x, y, z), value);
}

bool Map::isClipBox(std::int32_t x, std::int32_t y, std::int32_t z) const
{
    if (x < 0 || x >= 512 || y < 0 || y >= 512 || z >= 64)
    {
        return true;
    }
    if (z < 0)
    {
        return false;
    }
    if (z == 63)
    {
        z = 62;
    }
    return isSolid(static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y), static_cast<std::uint32_t>(z));
}

bool Map::isClipBoxF(float x, float y, float z) const
{
    return isClipBox(static_cast<std::int32_t>(std::floor(x)), static_cast<std::int32_t>(std::floor(y)), static_cast<std::int32_t>(std::floor(z)));
}

bool Map::isClipWorld(std::int32_t x, std::int32_t y, std::int32_t z) const
{
    if (x < 0 || x >= 512 || y < 0 || y >= 512 || z < 0)
    {
        return false;
    }
    if (z > 63)
    {
        return true;
    }
    if (z == 63)
    {
        z = 62;
    }
    return isSolid(static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y), static_cast<std::uint32_t>(z));
}

bool Map::isSurface(std::uint32_t offset) const
{
    if (!isSolid(offset))
    {
        return false;
    }

    // check above, below
    auto z = offset & 0x3F;
    if (z > 0 && !isSolid(offset - 1)) // check block above
    {
        return true;
    }
    if (z + 1 < SIZE_Z && !isSolid(offset + 1)) // check block below
    {
        return true;
    }

    // check left, right
    auto x = (offset >> 6) & 0x1FF;
    if (x > 0 && !isSolid(offset - SIZE_Z)) // check block on left
    {
        return true;
    }
    if (x + 1 < SIZE_X && !isSolid(offset + SIZE_Z)) // check block on right
    {
        return true;
    }

    // check up, down
    auto y = (offset >> 15) & 0x1FF;
    if (y > 0 && !isSolid(offset - SIZE_YZ)) // check block left
    {
        return true;
    }
    if (y + 1 < SIZE_Y && !isSolid(offset + SIZE_YZ)) // check block right
    {
        return true;
    }
    return false;
}

bool Map::isBlockWrap(std::int32_t x, std::int32_t y, std::int32_t z) const
{
    if (z < 0)
    {
        return false;
    }
    if (static_cast<std::uint32_t>(z) >= SIZE_Z)
    {
        return true;
    }
    return isSolid(static_cast<std::uint32_t>(x) & SIZE_MASK_X, static_cast<std::uint32_t>(y) & SIZE_MASK_X, static_cast<std::uint32_t>(z));
}

bool Map::hasNeighbor(std::uint32_t offset) const
{
    // check above, below
    auto z = offset & 0x3F;
    if (z > 0 && isSolid(offset - 1))
    { // check block above
        return true;
    }
    if (z + 1 < SIZE_Z && isSolid(offset + 1))
    { // check block below
        return true;
    }
    // check left, right
    auto x = (offset >> 6) & 0x1FF;
    if (x > 0 && isSolid(offset - SIZE_Z))
    { // check block on left
        return true;
    }
    if (x + 1 < SIZE_X && isSolid(offset + SIZE_Z))
    { // check block on right
        return true;
    }
    // check up, down
    auto y = (offset >> 15) & 0x1FF;
    if (y > 0 && isSolid(offset - SIZE_YZ))
    { // check block left
        return true;
    }
    if (y + 1 < SIZE_Y && isSolid(offset + SIZE_YZ))
    { // check block right
        return true;
    }
    return false;
}

std::uint32_t Map::getColor(std::uint32_t offset) const
{
    assert(offset < SIZE_XYZ);
    const Block & block  = mBlocks.at(offset);
    std::uint32_t color  = 0;
    color               |= static_cast<std::uint32_t>(block.b);
    color               |= static_cast<std::uint32_t>(block.g) << 8U;
    color               |= static_cast<std::uint32_t>(block.r) << 16U;
    color               |= static_cast<std::uint32_t>(0xFF) << 24U;
    return color;
}

void Map::setColor(std::uint32_t offset, std::uint32_t value)
{
    assert(offset < SIZE_XYZ);
    Block & block = mBlocks.at(offset);

    block.r       = static_cast<std::uint8_t>(value >> 16U);
    block.g       = static_cast<std::uint8_t>(value >> 8U);
    block.b       = static_cast<std::uint8_t>(value);
}

std::uint32_t Map::getHeight(std::uint32_t x, std::uint32_t y) const
{
    auto offset = Map::offset(x, y, 0);
    for (std::uint32_t z = 0; z < SIZE_Z; ++z)
    {
        if (isSolid(offset + z))
        {
            break;
        }
    }
    return SIZE_Z;
}

float Map::getHeightF(float x, float y) const
{
    return static_cast<float>(getHeight(static_cast<std::uint32_t>(std::floor(x)), static_cast<std::uint32_t>(std::floor(y))));
}

const std::uint8_t * Map::readColumnFromMemory(const std::uint8_t * data, std::uint32_t & offset)
{
    for (std::uint32_t z = 0;;)
    {
        std::uint8_t span_size = data[0]; // N
        std::uint8_t top_start = data[1]; // S
        std::uint8_t top_end   = data[2]; // E
        // std::uint8_t air_start = data[3]; // A - unused

        // air
        for (; z < top_start; ++z)
        {
            setSolid(offset + z, false);
        }

        // number of top blocks
        std::uint8_t top_length = top_end - top_start + 1; // K = S - E + 1

        // top
        const auto * colors = reinterpret_cast<const std::uint32_t *>(data + 4);
        for (; z <= top_end; ++z, ++colors)
        {
            // m_blocks.set(offset + z, true);
            setColor(offset + z, *colors);
        }

        if (span_size == 0)
        { // last span in column
            data += static_cast<ptrdiff_t>(4 * (top_length + 1));
            break;
        }

        // number of bottom blocks
        std::uint8_t bottom_length = (span_size - 1) - top_length; // Z = (N - 1) - K
        // move to the next span
        data += static_cast<ptrdiff_t>(span_size * 4);
        // bottom ends where air begins

        std::uint8_t bottom_end   = data[3];                    // (M - 1) - block end, M - next span air
        std::uint8_t bottom_start = bottom_end - bottom_length; // M -  Z

        // bottom
        for (z = bottom_start; z < bottom_end; ++z, ++colors)
        {
            // m_blocks.set(offset + z, true);
            setColor(offset + z, *colors);
        }
    }
    offset += SIZE_Z;
    return data;
}

void Map::readFromMemory(const std::uint8_t * data)
{
    std::uint32_t offset = 0;

    mBlocks.fill({ DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B, 0 });
    while (offset < SIZE_XYZ)
    {
        data = readColumnFromMemory(data, offset);
    }
}

void Map::writeColumnToMemory(std::vector<std::uint8_t> & result, std::uint32_t & offset)
{
    for (std::uint32_t z = 0; z < SIZE_Z;)
    {
        // air
        std::uint8_t air_start = z;
        for (; z < SIZE_Z && !isSolid(offset + z); ++z)
        {
        }

        // top
        std::uint8_t top_start = z;
        for (; z < SIZE_Z && isSurface(offset + z); ++z)
        {
        }
        std::uint8_t top_end = z;

        // not visible blocks
        for (; z < SIZE_Z && isSolid(offset + z) && !isSurface(offset + z); ++z)
        {
        }

        // bottom
        auto         t            = z;
        std::uint8_t bottom_start = z;

        for (; t < SIZE_Z && isSurface(offset + t); ++t)
        {
        }

        if (t != SIZE_Z)
        {
            for (; isSurface(offset + z); ++z)
            {
            }
        }

        std::uint8_t bottom_end = z;

        // result
        std::uint8_t top_length    = top_end - top_start;
        std::uint8_t bottom_length = bottom_end - bottom_start;
        std::uint8_t colors_length = top_length + bottom_length;

        if (z == SIZE_Z)
        {
            result.push_back(0);
        }
        else
        {
            result.push_back(colors_length + 1);
        }

        result.push_back(top_start);
        result.push_back(top_end - 1);
        result.push_back(air_start);

        for (std::uint32_t i = top_start; i < top_end; ++i)
        {
            auto & block = mBlocks.at(offset + i);
            result.push_back(DEFAULT_COLOR_A);
            result.push_back(block.r);
            result.push_back(block.g);
            result.push_back(block.b);
        }

        for (std::uint32_t i = bottom_start; i < bottom_end; ++i)
        {
            auto & block = mBlocks.at(offset + i);
            result.push_back(DEFAULT_COLOR_A);
            result.push_back(block.r);
            result.push_back(block.g);
            result.push_back(block.b);
        }
    }

    offset += SIZE_Z;
}

void Map::writeToMemory(std::vector<std::uint8_t> & result)
{
    std::uint32_t offset = 0;

    result.clear();
    result.reserve(static_cast<std::size_t>(SIZE_XY) * (4 + 4)); // reserve at least 8 bytes per column (header + one color)

    while (offset < SIZE_XYZ)
    {
        writeColumnToMemory(result, offset);
    }
}

} // namespace cxxserver
