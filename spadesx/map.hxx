/**
 * @file map.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "baseconnection.hxx"

#include <bitset>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/stream.hpp>
#include <cmath>
#include <vector>

namespace bio = boost::iostreams;

namespace spadesx {

/**
 * @brief Game map
 *
 */
class map
{
  public:
    static constexpr const std::uint32_t size_x        = 512;              //!< Max x-coordinate
    static constexpr const std::uint32_t size_y        = 512;              //!< Max y-coordinate
    static constexpr const std::uint32_t size_z        = 64;               //!< Max z-coordinate
    static constexpr const std::uint32_t size_xy       = size_x * size_y;  //!< Horizontal area
    static constexpr const std::uint32_t size_yz       = size_y * size_z;  //!< Vertical area
    static constexpr const std::uint32_t size_xyz      = size_xy * size_z; //!< Max number of blocks
    static constexpr const std::uint32_t default_color = 0xFF674028;       //!< Default color

    /**
     * @brief Construct a new map object
     *
     */
    map() = default;

    /**
     * @brief Destroy the map object
     *
     */
    ~map() = default;

    /**
     * @brief Returns true if map has been modified since last compression
     *
     * @return true If map has been modified since last compression
     */
    [[nodiscard]] bool has_changed() const noexcept
    {
        return m_changed;
    }

    /**
     * @brief Modify single block
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @param value The new state of the block
     * @param color Block color in ARGB format
     */
    void modify_block(std::uint32_t x, std::uint32_t y, std::uint32_t z, bool value, std::uint32_t color)
    {
        if (z > 61) {
            return; // bottom blocks cannot be modified
        }
        auto offset = get_offset(x, y, z);
        assert(offset < size_xyz);
        set_block(offset, value);
        set_color(offset, color);
        m_changed = true;
    }

    /**
     * @brief Destroy single block
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     */
    void destroy_block(std::uint32_t x, std::uint32_t y, std::uint32_t z)
    {
        if (z > 61) {
            return; // bottom blocks cannot be broken
        }
        set_block(x, y, z, false);
        m_changed = true;
    }

    /**
     * @brief Destroy 3 block
     *
     * @param x The x-coordinate of the middle block
     * @param y The y-coordinate of the middle block
     * @param z The z-coordinate of the middle block
     */
    void destroy_block_secondary(std::uint32_t x, std::uint32_t y, std::uint32_t z)
    {
        if (z > 61) {
            return; // bottom blocks cannot be broken
        }
        auto offset = get_offset(x, y, z);
        if (z > 0) {
            set_block(offset - 1, false);
        }
        if (z < 61) {
            set_block(offset + 1, false);
        }
        set_block(offset, false);
        m_changed = true;
    }

    /**
     * @brief Check whether the block at the given index is solid
     *
     * @param offset The offset
     * @return true If the block is solid
     * @return false If the block is not solid, i.e. is air
     */
    [[nodiscard]] bool is_block(std::uint32_t offset) const
    {
        assert(offset < size_xyz);
        return m_blocks.test(offset);
    }

    /**
     * @brief Set the state of the block on the given offset
     *
     * @param offset The offset
     * @param value The new state of the block
     */
    void set_block(std::uint32_t offset, bool value)
    {
        assert(offset < size_xyz);
        m_blocks.set(offset, value);
    }

    /**
     * @brief Check whether the block is a surface block (is visible from at least one side)
     *
     * @param offset The offset
     * @return true If the block is a surface block
     * @return false If the block is not solid or if is not a surface block
     */
    [[nodiscard]] bool is_surface(std::uint32_t offset) const
    {
        if (!is_block(offset)) {
            return false;
        }
        // check above, below
        auto z = offset & 0x3F;
        if (z > 0 && !is_block(offset - 1)) { // check block above
            return true;
        }
        if (z + 1 < size_z && !is_block(offset + 1)) { // check block below
            return true;
        }
        // check left, right
        auto x = (offset >> 6) & 0x1FF;
        if (x > 0 && !is_block(offset - size_z)) { // check block on left
            return true;
        }
        if (x + 1 < size_x && !is_block(offset + size_z)) { // check block on right
            return true;
        }
        // check up, down
        auto y = (offset >> 15) & 0x1FF;
        if (y > 0 && !is_block(offset - size_yz)) { // check block left
            return true;
        }
        if (y + 1 < size_y && !is_block(offset + size_yz)) { // check block right
            return true;
        }
        return false;
    }

    /**
     * @brief Same as is_block, but water is empty and out of bounds returns true
     *
     * @param x The x-coordinate of ...
     * @param y The y-coordinate of ...
     * @param z The z-coordinate of ...
     * @return true ...
     */
    bool is_clip_box(float x, float y, float z) const
    {
        if (x < 0.F || x >= 512.F || y < 0.F || y >= 512.F || z >= 64.F) {
            return true;
        }
        if (z < 0.F) {
            return false;
        }
        auto _z = static_cast<std::uint32_t>(std::floor(z));
        if (_z == 63) {
            _z = 62;
        }
        return is_block(static_cast<std::uint32_t>(std::floor(x)), static_cast<std::uint32_t>(std::floor(y)), _z);
    }

    /**
     * @brief Check whether the block (or air) has a neighbor (another solid block attached)
     *
     * @param offset The offset
     * @return true If the block has a neighbor
     * @return false If the block does not have a neighbor
     */
    [[nodiscard]] bool has_neighbor(std::uint32_t offset) const
    {
        // check above, below
        auto z = offset & 0x3F;
        if (z > 0 && is_block(offset - 1)) { // check block above
            return true;
        }
        if (z + 1 < size_z && is_block(offset + 1)) { // check block below
            return true;
        }
        // check left, right
        auto x = (offset >> 6) & 0x1FF;
        if (x > 0 && is_block(offset - size_z)) { // check block on left
            return true;
        }
        if (x + 1 < size_x && is_block(offset + size_z)) { // check block on right
            return true;
        }
        // check up, down
        auto y = (offset >> 15) & 0x1FF;
        if (y > 0 && is_block(offset - size_yz)) { // check block left
            return true;
        }
        if (y + 1 < size_y && is_block(offset + size_yz)) { // check block right
            return true;
        }
        return false;
    }

    /**
     * @brief Check whether the block (or air) has a neighbor (another solid block attached)
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return true If the block has a neighbor
     * @return false If the block does not have a neighbor
     */
    [[nodiscard]] bool has_neighbor(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return has_neighbor(get_offset(x, y, z));
    }

    /**
     * @brief Get the color of the block on the given offset
     *
     * @param offset The offset
     * @return Block color in ARGB format
     */
    [[nodiscard]] std::uint32_t get_color(std::uint32_t offset) const
    {
        assert(offset < size_xyz);
        return m_colors[offset];
    }

    /**
     * @brief Set the color of the block on the given offset
     *
     * @param offset The offset
     * @param value Block color in ARGB format
     */
    void set_color(std::uint32_t offset, std::uint32_t value)
    {
        assert(offset < size_xyz);
        m_colors[offset] = value;
    }

    /**
     * @brief Calculate the offset of the block from the coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return The offset of the block
     */
    static std::uint32_t get_offset(std::uint32_t x, std::uint32_t y, std::uint32_t z)
    {
        return (((y << 9) + x) << 6) + z;
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
    [[nodiscard]] bool is_block(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return is_block(get_offset(x, y, z));
    }

    /**
     * @brief Set the state of the block on the given coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @param value The new state of the block
     */
    void set_block(std::uint32_t x, std::uint32_t y, std::uint32_t z, bool value)
    {
        set_block(get_offset(x, y, z), value);
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
    [[nodiscard]] bool is_surface(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return is_surface(get_offset(x, y, z));
    }

    /**
     * @brief Get the color of the block on the given coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @return Block color in ARGB format
     */
    [[nodiscard]] std::uint32_t get_color(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return get_color(get_offset(x, y, z));
    }

    /**
     * @brief Set the color of the block on the given coordinates
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     * @param value Block color in ARGB format
     */
    void set_color(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t value)
    {
        set_color(get_offset(x, y, z), value);
    }

    /**
     * @brief Get the height at which the highest-placed block is located
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @return The z-coordinate of the block
     */
    [[nodiscard]] std::uint32_t get_height(std::uint32_t x, std::uint32_t y) const
    {
        auto          offset = get_offset(x, y, 0);
        std::uint32_t z;
        for (z = 0; z < 64 && !m_blocks.test(offset + z); ++z) {
        }
        return z;
    }

    /**
     * @brief Get the height at which the highest-placed block is located
     *
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @return The z-coordinate of the block
     */
    [[nodiscard]] float get_height(float x, float y) const
    {
        return static_cast<float>(get_height(static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)));
    }

    /**
     * @brief Read single column from memory to offset
     *
     * @param data Data
     * @param offset Offset
     * @return Next memory location
     */
    const std::uint8_t* read_column_from_memory(const std::uint8_t* data, std::uint32_t& offset)
    {
        for (std::uint32_t z = 0;;) {

            std::uint8_t span_size = data[0]; // N
            std::uint8_t top_start = data[1]; // S
            std::uint8_t top_end   = data[2]; // E
            std::uint8_t air_start = data[3]; // A

            // air
            for (; z < top_start; ++z) {
                m_blocks.set(offset + z, false);
            }

            // number of top blocks
            std::uint8_t top_length = top_end - top_start + 1; // K = S - E + 1

            // top
            const auto* colors = reinterpret_cast<const std::uint32_t*>(data + 4);
            for (; z <= top_end; ++z, ++colors) {
                // m_blocks.set(offset + z, true);
                m_colors.at(offset + z) = *colors;
            }

            if (span_size == 0) { // last span in column
                data += 4 * (top_length + 1);
                break;
            }

            // number of bottom blocks
            std::uint8_t bottom_length = (span_size - 1) - top_length; // Z = (N - 1) - K
            // move to the next span
            data += span_size * 4;
            // bottom ends where air begins

            std::uint8_t bottom_end   = data[3];                    // (M - 1) - block end, M - next span air
            std::uint8_t bottom_start = bottom_end - bottom_length; // M -  Z

            // bottom
            for (z = bottom_start; z < bottom_end; ++z, ++colors) {
                // m_blocks.set(offset + z, true);
                m_colors.at(offset + z) = *colors;
            }
        }
        offset += 64;
        return data;
    }

    /**
     * @brief Load map from memory (VXL format)
     *
     * @param data Beginning of the memory region
     */
    void read_from_memory(const std::uint8_t* data)
    {
        std::uint32_t offset = 0;

        m_blocks.set();
        m_colors.fill(default_color);

        while (offset < size_xyz) {
            data = read_column_from_memory(data, offset);
        }
    }

    /**
     * @brief Load map from file (VXL format)
     *
     * @param filepath Path to the file (VXL format file)
     */
    void read_from_file(std::string_view filepath)
    {
        boost::iostreams::mapped_file_source source(std::string{filepath});
        if (!source.is_open()) {
            throw std::runtime_error("failed to open map file");
        }

        const auto* data = reinterpret_cast<const std::uint8_t*>(source.data());
        read_from_memory(data);
    }

    /**
     * @brief Write single column from offset to memory
     *
     * @param result Result
     * @param offset Offset
     */
    void write_column_to_memory(std::vector<std::uint8_t>& result, std::uint32_t& offset)
    {
        for (std::uint32_t z = 0; z < size_z;) {
            // air
            std::uint8_t air_start = z;
            for (; z < size_z && !is_block(offset + z); ++z) {
            }

            // top
            std::uint8_t top_start = z;
            for (; z < size_z && is_surface(offset + z); ++z) {
            }
            std::uint8_t top_end = z;

            // not visible blocks
            for (; z < size_z && is_block(offset + z) && !is_surface(offset + z); ++z) {
            }

            // bottom
            auto         t            = z;
            std::uint8_t bottom_start = z;

            for (; t < size_z && is_surface(offset + t); ++t) {
            }

            if (t != size_z) {
                for (; is_surface(offset + z); ++z) {
                }
            }

            std::uint8_t bottom_end = z;

            // result
            std::uint8_t top_length    = top_end - top_start;
            std::uint8_t bottom_length = bottom_end - bottom_start;
            std::uint8_t colors_length = top_length + bottom_length;

            if (z == size_z) {
                result.push_back(0);
            } else {
                result.push_back(colors_length + 1);
            }

            result.push_back(top_start);
            result.push_back(top_end - 1);
            result.push_back(air_start);

            for (std::uint32_t i = top_start; i < top_end; ++i) {
                auto color = m_colors[offset + i];
                result.push_back(color & 0xFF);
                result.push_back((color >> 8) & 0xFF);
                result.push_back((color >> 16) & 0xFF);
                result.push_back((color >> 24) & 0xFF);
            }

            for (std::uint32_t i = bottom_start; i < bottom_end; ++i) {
                auto color = m_colors[offset + i];
                result.push_back(color & 0xFF);
                result.push_back((color >> 8) & 0xFF);
                result.push_back((color >> 16) & 0xFF);
                result.push_back((color >> 24) & 0xFF);
            }
        }
        offset += 64;
    }

    /**
     * @brief Write map to memory (to VXL format)
     *
     * @param result Output (vector)
     */
    void write_to_memory(std::vector<std::uint8_t>& result)
    {
        std::uint32_t offset = 0;

        result.clear();
        result.reserve(512 * 512 * 8); // reserve at least 8 bytes per column (header + one color)

        while (offset < size_xyz) {
            write_column_to_memory(result, offset);
        }
    }

    /**
     * @brief Write map to memory and compress
     *
     * @param result Result buffer
     * @todo Rewrite write function to stream
     */
    void write_and_compress(std::vector<char>& result)
    {
        std::vector<std::uint8_t> data;
        write_to_memory(data);

        result.clear();
        result.reserve(512 * 512);

        try {
            auto*                          data_in = reinterpret_cast<char*>(data.data());
            bio::stream<bio::array_source> in(data_in, data.size());

            bio::filtering_streambuf<bio::input> filter;
            filter.push(boost::iostreams::zlib_compressor());
            filter.push(in);

            bio::back_insert_device<std::vector<char>>              out_device(result);
            bio::stream<bio::back_insert_device<std::vector<char>>> out(out_device);

            bio::copy(filter, out);
        } catch (const bio::zlib_error& ex) {
            auto error = ex.error();
            std::cerr << "zlib error: " << error << std::endl;
        } catch (...) {
            std::cerr << "unknown zlib error" << std::endl;
        }

        m_changed = false;
    }

  private:
    std::bitset<size_xyz>               m_blocks;        //!< Bitset of blocks (solid/not solid)
    std::array<std::uint32_t, size_xyz> m_colors;        //!< Array of colors
    bool                                m_changed{true}; //!< Has map changed since last compression?
};

} // namespace spadesx
