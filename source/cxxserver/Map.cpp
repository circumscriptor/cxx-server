#include "Map.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <glm/ext/vector_uint3.hpp>
#include <span>
#include <vector>

namespace cxxserver {

void Map::modify_block(glm::uvec3 coords, bool value, std::uint32_t color)
{
    if (coords.z > 61) {
        return; // bottom blocks cannot be modified
    }
    auto offset = Map::to_offset(coords);
    assert(offset < SIZE_XYZ);
    set_solid(offset, value);
    set_color(offset, color);
    m_changed = true;
}

void Map::destroy_block(glm::uvec3 coords)
{
    if (coords.z > 61) {
        return; // bottom blocks cannot be broken
    }
    set_solid(coords, false);
    m_changed = true;
}

void Map::destroy_block_secondary(glm::uvec3 coords)
{
    if (coords.z > 61) {
        return; // bottom blocks cannot be broken
    }
    auto offset = Map::to_offset(coords);
    if (coords.z > 0) {
        set_solid(offset - 1, false);
    }
    if (coords.z < 61) {
        set_solid(offset + 1, false);
    }
    set_solid(offset, false);
    m_changed = true;
}

void Map::destroy_block_grenade(glm::ivec3 coords)
{
    for (int dz = -1; dz < 2; ++dz) {
        auto zCheck = coords.z + dz;
        if (zCheck < 0 || zCheck > 61) {
            continue;
        }
        for (int dy = -1; dy < 2; ++dy) {
            auto yCheck = coords.y + dy;
            if (yCheck < 0 || yCheck >= 512) {
                continue;
            }
            for (int dx = -1; dx < 2; ++dx) {
                auto xCheck = coords.x + dx;
                if (xCheck < 0 || xCheck >= 512) {
                    continue;
                }
                set_solid(coords, false);
                m_changed = true;
            }
        }
    }
}

void Map::set_solid(std::uint32_t offset, bool value)
{
    assert(offset < SIZE_XYZ);
    if (value) {
        m_blocks.at(offset).data |= BIT_SOLID;
    } else {
        m_blocks.at(offset).data &= ~BIT_SOLID; // NOLINT(hicpp-signed-bitwise) ??
    }
}

bool Map::is_solid(std::uint32_t offset) const
{
    assert(offset < SIZE_XYZ);
    return (m_blocks.at(offset).data & BIT_SOLID) != 0;
}

void Map::set_solid(glm::uvec3 coords, bool value)
{
    set_solid(to_offset(coords), value);
}

bool Map::is_clip_box(glm::ivec3 coords) const
{
    if (coords.x < 0 || coords.x >= 512 || coords.y < 0 || coords.y >= 512 || coords.z >= 64) {
        return true;
    }
    if (coords.z < 0) {
        return false;
    }
    if (coords.z == 63) {
        coords.z = 62;
    }
    return is_solid(coords);
}

bool Map::is_clip_box_f(glm::vec3 coords) const
{
    return is_clip_box(glm::floor(coords));
}

bool Map::is_clip_world(glm::ivec3 coords) const
{
    if (coords.x < 0 || coords.x >= 512 || coords.y < 0 || coords.y >= 512 || coords.z >= 64) {
        return false;
    }
    if (coords.z < 0) {
        return false;
    }
    if (coords.z == 63) {
        coords.z = 62;
    }
    return is_solid(coords);
}

bool Map::is_surface(std::uint32_t offset) const
{
    if (!is_solid(offset)) {
        return false;
    }

    glm::uvec3 coords = from_offset(offset);

    // check left, right
    if (coords.x > 0 && !is_solid(offset - SIZE_Z)) // check block on left
    {
        return true;
    }
    if (coords.x + 1 < SIZE_X && !is_solid(offset + SIZE_Z)) // check block on right
    {
        return true;
    }

    // check up, down
    if (coords.y > 0 && !is_solid(offset - SIZE_YZ)) // check block left
    {
        return true;
    }
    if (coords.y + 1 < SIZE_Y && !is_solid(offset + SIZE_YZ)) // check block right
    {
        return true;
    }

    // check above, below
    if (coords.z > 0 && !is_solid(offset - 1)) // check block above
    {
        return true;
    }
    if (coords.z + 1 < SIZE_Z && !is_solid(offset + 1)) // check block below
    {
        return true;
    }
    return false;
}

bool Map::is_block_wrap(glm::ivec3 coords) const
{
    if (coords.z < 0) {
        return false;
    }
    if (static_cast<std::uint32_t>(coords.z) >= SIZE_Z) {
        return true;
    }
    return is_solid(glm::uvec3{coords} & glm::uvec3{MASK_X, MASK_Y, ~0U});
}

bool Map::has_neighbor(std::uint32_t offset) const
{
    glm::uvec3 coords = from_offset(offset);

    // check left, right
    if (coords.x > 0 && is_solid(offset - SIZE_Z)) { // check block on left
        return true;
    }
    if (coords.x + 1 < SIZE_X && is_solid(offset + SIZE_Z)) { // check block on right
        return true;
    }

    // check up, down
    if (coords.y > 0 && is_solid(offset - SIZE_YZ)) { // check block left
        return true;
    }
    if (coords.y + 1 < SIZE_Y && is_solid(offset + SIZE_YZ)) { // check block right
        return true;
    }

    // check above, below
    if (coords.z > 0 && is_solid(offset - 1)) { // check block above
        return true;
    }
    if (coords.z + 1 < SIZE_Z && is_solid(offset + 1)) { // check block below
        return true;
    }
    return false;
}

std::uint32_t Map::get_color(std::uint32_t offset) const
{
    assert(offset < SIZE_XYZ);
    const Block&  block  = m_blocks.at(offset);
    std::uint32_t color  = 0;
    color               |= static_cast<std::uint32_t>(block.b);
    color               |= static_cast<std::uint32_t>(block.g) << 8U;
    color               |= static_cast<std::uint32_t>(block.r) << 16U;
    color               |= static_cast<std::uint32_t>(0xFF) << 24U;
    return color;
}

void Map::set_color(std::uint32_t offset, std::uint32_t value)
{
    assert(offset < SIZE_XYZ);
    Block& block = m_blocks.at(offset);

    block.r      = static_cast<std::uint8_t>(value >> 16U);
    block.g      = static_cast<std::uint8_t>(value >> 8U);
    block.b      = static_cast<std::uint8_t>(value);
}

std::uint32_t Map::get_height(glm::uvec2 coords) const
{
    auto offset = Map::to_offset({coords, 0});
    for (std::uint32_t zIndex = 0; zIndex < SIZE_Z; ++zIndex) {
        if (is_solid(offset + zIndex)) {
            break;
        }
    }
    return SIZE_Z;
}

float Map::get_height_f(glm::vec2 coords) const
{
    return static_cast<float>(get_height(glm::floor(coords)));
}

std::span<std::byte> Map::read_column_from_memory(std::span<std::byte> data, std::uint32_t& offset)
{
    for (std::uint8_t zOffset = 0;;) {
        auto spanSize = static_cast<std::uint8_t>(data[0]); // N
        auto topStart = static_cast<std::uint8_t>(data[1]); // S
        auto topEnd   = static_cast<std::uint8_t>(data[2]); // E
        // auto airStart = static_cast<std::uint8_t>(data[3]); // A - unused

        // air
        for (; zOffset < topStart; ++zOffset) {
            set_solid(offset + zOffset, false);
        }

        // number of top blocks
        std::uint8_t topLength = topEnd - topStart + 1; // K = S - E + 1

        // top
        auto                     colorBytes = data.subspan(4);
        std::span<std::uint32_t> colors{reinterpret_cast<std::uint32_t*>(colorBytes.data()), colorBytes.size() / 4}; // NOLINT
        for (auto i = 0UZ; zOffset <= topEnd; ++zOffset, ++i) {
            // m_blocks.set(offset + z, true);
            set_color(offset + zOffset, colors[i]);
        }

        if (spanSize == 0) { // last span in column
            data = data.subspan(4UZ * (topLength + 1));
            break;
        }

        // number of bottom blocks
        std::uint8_t bottomLength = (spanSize - 1) - topLength; // Z = (N - 1) - K
        // move to the next span
        data = data.subspan(spanSize * 4UZ);
        // bottom ends where air begins

        auto         bottomEnd   = static_cast<std::uint8_t>(data[3]); // (M - 1) - block end, M - next span air
        std::uint8_t bottomStart = bottomEnd - bottomLength;           // M -  Z

        // bottom
        zOffset = bottomStart;
        for (auto i = topEnd; zOffset < bottomEnd; ++zOffset, ++i) {
            // m_blocks.set(offset + z, true);
            set_color(offset + zOffset, colors[i]);
        }
    }
    offset += SIZE_Z;
    return data;
}

void Map::read_from_memory(std::span<std::byte> data)
{
    std::uint32_t offset = 0;
    m_blocks.fill({DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B, 0});
    while (not data.empty() && offset < SIZE_XYZ) {
        data = read_column_from_memory(data, offset);
    }
}

void Map::write_column_to_memory(std::vector<std::uint8_t>& result, std::uint32_t& offset)
{
    for (std::uint8_t zOffset = 0; zOffset < SIZE_Z;) {
        // air
        std::uint8_t airStart = zOffset;
        for (; zOffset < SIZE_Z && !is_solid(offset + zOffset); ++zOffset) { }

        // top
        std::uint8_t topStart = zOffset;
        for (; zOffset < SIZE_Z && is_surface(offset + zOffset); ++zOffset) { }
        std::uint8_t topEnd = zOffset;

        // not visible blocks
        for (; zOffset < SIZE_Z && is_solid(offset + zOffset) && !is_surface(offset + zOffset); ++zOffset) { }

        // bottom
        auto         temp        = zOffset;
        std::uint8_t bottomStart = zOffset;

        for (; temp < SIZE_Z && is_surface(offset + temp); ++temp) { }

        if (temp != SIZE_Z) {
            for (; is_surface(offset + zOffset); ++zOffset) { }
        }

        std::uint8_t bottomEnd = zOffset;

        // result
        std::uint8_t topLength    = topEnd - topStart;
        std::uint8_t bottomLength = bottomEnd - bottomStart;
        std::uint8_t colorsLength = topLength + bottomLength;

        if (zOffset == SIZE_Z) {
            result.push_back(0);
        } else {
            result.push_back(colorsLength + 1);
        }

        result.push_back(topStart);
        result.push_back(topEnd - 1);
        result.push_back(airStart);

        for (std::uint32_t i = topStart; i < topEnd; ++i) {
            auto& block = m_blocks.at(offset + i);
            result.push_back(DEFAULT_COLOR_A);
            result.push_back(block.r);
            result.push_back(block.g);
            result.push_back(block.b);
        }

        for (std::uint32_t i = bottomStart; i < bottomEnd; ++i) {
            auto& block = m_blocks.at(offset + i);
            result.push_back(DEFAULT_COLOR_A);
            result.push_back(block.r);
            result.push_back(block.g);
            result.push_back(block.b);
        }
    }

    offset += SIZE_Z;
}

void Map::write_to_memory(std::vector<std::uint8_t>& result)
{
    std::uint32_t offset = 0;

    result.clear();
    result.reserve(static_cast<std::size_t>(SIZE_XY) * (4 + 4)); // reserve at least 8 bytes per column (header + one color)

    while (offset < SIZE_XYZ) {
        write_column_to_memory(result, offset);
    }
}

} // namespace cxxserver
