#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <glm/ext/vector_uint3.hpp>
#include <span>
#include <vector>

namespace cxxserver {

class Map {
public:

    struct Block {
        std::uint8_t b;
        std::uint8_t g;
        std::uint8_t r;
        std::uint8_t data;
    };

    static_assert(sizeof(Block) == 4);

    static const std::uint32_t SIZE_X          = 512;              //!< Max x-coordinate
    static const std::uint32_t SIZE_Y          = 512;              //!< Max y-coordinate
    static const std::uint32_t SIZE_Z          = 64;               //!< Max z-coordinate
    static const std::uint32_t SIZE_XY         = SIZE_X * SIZE_Y;  //!< Horizontal area
    static const std::uint32_t SIZE_YZ         = SIZE_Y * SIZE_Z;  //!< Vertical area
    static const std::uint32_t SIZE_XYZ        = SIZE_XY * SIZE_Z; //!< Max number of blocks
    static const std::uint8_t  DEFAULT_COLOR_A = 0xFF;             //!< Default color A component
    static const std::uint8_t  DEFAULT_COLOR_R = 0x67;             //!< Default color R component
    static const std::uint8_t  DEFAULT_COLOR_G = 0x40;             //!< Default color G component
    static const std::uint8_t  DEFAULT_COLOR_B = 0x28;             //!< Default color B component
    static const std::uint32_t MASK_X          = SIZE_X - 1;       //!< Mask for horizontal coordinates
    static const std::uint32_t MASK_Y          = SIZE_Y - 1;       //!< Mask for horizontal coordinates
    static const std::uint32_t MASK_Z          = SIZE_Z - 1;       //!< Mask for vertical coordinates
    static const std::uint32_t BITS_X          = 9;                //!< Bits for x-coordinate
    static const std::uint32_t BITS_Y          = 9;                //!< Bits for y-coordinate
    static const std::uint32_t BITS_Z          = 6;                //!< Bits for z-coordinate
    static const std::uint32_t SHIFT_X         = BITS_Z + BITS_Y;
    static const std::uint32_t SHIFT_Y         = BITS_Z;
    static const std::uint32_t SHIFT_Z         = 0;
    static const std::uint32_t SHIFT_R         = 16;
    static const std::uint32_t SHIFT_G         = 8;
    static const std::uint32_t SHIFT_B         = 0;
    static const std::uint32_t SHIFT_A         = 24;

    // Bits
    static const std::uint8_t BIT_SOLID = 0x01; //!< Block data bit: is solid

    // Block limits
    static const std::int32_t LIMIT_BREAKABLE    = 61;
    static const std::int32_t LIMIT_GROUND_LEVEL = 62;
    static const std::int32_t LIMIT_WATER_LEVEL  = 63;
    static const std::int32_t LIMIT_MAX_X        = SIZE_X;
    static const std::int32_t LIMIT_MAX_Y        = SIZE_Y;
    static const std::int32_t LIMIT_MAX_Z        = SIZE_Z;

    ///
    /// @brief Construct a new map object
    ///
    ///
    Map()                      = default;

    Map(const Map&)            = default;
    Map(Map&&)                 = delete;
    Map& operator=(const Map&) = default;
    Map& operator=(Map&&)      = delete;

    ///
    /// @brief Destroy the map object
    ///
    ///
    ~Map() = default;

    ///
    /// @brief Returns true if map has been modified since last compression
    ///
    /// @return true If map has been modified since last compression
    ///
    [[nodiscard]]
    bool has_changed() const noexcept
    {
        return m_changed;
    }

    ///
    /// @brief Modify single block
    ///
    /// @param coords Coordinates of the block
    /// @param value The new state of the block
    /// @param color Block color in ARGB format
    ///
    void modify_block(glm::uvec3 coords, bool value, std::uint32_t color);

    ///
    /// @brief Destroy single block
    ///
    /// @param coords Coordinates of the block
    ///
    void destroy_block(glm::uvec3 coords);

    ///
    /// @brief Destroy 3 block
    ///
    /// @param coords Coordinates of the middle block
    ///
    void destroy_block_secondary(glm::uvec3 coords);

    ///
    /// @brief Destroy up-to 27 blocks
    ///
    /// @param coords Coordinates of the grenade explosion
    ///
    void destroy_block_grenade(glm::ivec3 coords);

    ///
    /// @brief Check whether the block at the given index is solid
    ///
    /// @param offset The offset
    /// @return true If the block is solid
    /// @return false If the block is not solid, i.e. is air
    ///
    [[nodiscard]]
    bool is_solid(std::uint32_t offset) const;

    ///
    /// @brief Set the state of the block on the given offset
    ///
    /// @param offset The offset
    /// @param value The new state of the block
    ///
    void set_solid(std::uint32_t offset, bool value);

    ///
    /// @brief Check whether the block is a surface block (is visible from at least one side)
    ///
    /// @param offset The offset
    /// @return true If the block is a surface block
    /// @return false If the block is not solid or if is not a surface block
    ///
    [[nodiscard]]
    bool is_surface(std::uint32_t offset) const;

    ///
    /// @brief Same as is_solid, but water is empty and out of bounds returns true
    ///
    /// @param coords Coordinates of the block
    /// @return true ...
    ///
    [[nodiscard]]
    bool is_clip_box(glm::ivec3 coords) const;

    ///
    /// @brief Same as is_solid, but water is empty and out of bounds returns false
    ///
    /// @param coords Coordinates of the block
    /// @return true ...
    ///
    [[nodiscard]]
    bool is_clip_world(glm::ivec3 coords) const;

    ///
    /// @brief Same as is_block, but water is empty and out of bounds returns true
    ///
    /// @param coords Coordinates of the block
    /// @return true ...
    ///
    [[nodiscard]]
    bool is_clip_box_f(glm::vec3 coords) const;

    ///
    /// @brief Check whether the block (or air) has a neighbor (another solid block attached)
    ///
    /// @param offset The offset
    /// @return true If the block has a neighbor
    /// @return false If the block does not have a neighbor
    ///
    [[nodiscard]]
    bool has_neighbor(std::uint32_t offset) const;

    ///
    /// @brief Check whether the block (or air) has a neighbor (another solid block attached)
    ///
    /// @param coords Coordinates of the block
    /// @return true If the block has a neighbor
    /// @return false If the block does not have a neighbor
    ///
    [[nodiscard]]
    bool has_neighbor(glm::uvec3 coords) const
    {
        return has_neighbor(to_offset(coords));
    }

    ///
    /// @brief Get the color of the block on the given offset
    ///
    /// @param offset The offset
    /// @return Block color in ARGB format
    ///
    [[nodiscard]]
    std::uint32_t get_color(std::uint32_t offset) const;

    ///
    /// @brief Set the color of the block on the given offset
    ///
    /// @param offset The offset
    /// @param value Block color in ARGB format
    ///
    void set_color(std::uint32_t offset, std::uint32_t value);

    ///
    /// @brief Calculate the offset of the block from the coordinates
    ///
    /// @param coords Coordinates of the block
    /// @return The offset of the block
    ///
    static constexpr std::uint32_t to_offset(glm::uvec3 coords) noexcept
    {
        return (((coords.x << BITS_Y) + coords.y) << BITS_Z) + coords.z;
    }

    ///
    /// @brief Calculate coordinates of the block from the offset
    ///
    /// @param offset The offset
    /// @return Coordinates of the block
    ///
    static constexpr glm::uvec3 from_offset(std::uint32_t offset) noexcept
    {
        return {(offset >> SHIFT_X) & MASK_X, (offset >> SHIFT_Y) & MASK_Y, (offset >> SHIFT_Z) & MASK_Z};
    }

    ///
    /// @brief Check whether the block at the given coordinates is solid
    ///
    /// @param coords Coordinates of the block
    /// @return true If the block is solid
    /// @return false If the block is not solid, i.e. is air
    ///
    [[nodiscard]]
    bool is_solid(glm::uvec3 coords) const
    {
        return is_solid(to_offset(coords));
    }

    ///
    /// @brief Check whether the block at the given coordinates is solid (with wrapping)
    ///
    /// @param coords Coordinates of the block
    /// @return true If the block is solid
    /// @return false If the block is not solid, i.e. is air
    ///
    [[nodiscard]]
    bool is_block_wrap(glm::ivec3 coords) const;

    ///
    /// @brief Set the state of the block on the given coordinates
    ///
    /// @param coords Coordinates of the block
    /// @param value The new state of the block
    ///
    void set_solid(glm::uvec3 coords, bool value);

    ///
    /// @brief Check whether the block is a surface block (is visible from at least one side)
    ///
    /// @param coords Coordinates of the block
    /// @return true If the block is a surface block
    /// @return false If the block is not solid or if is not a surface block
    ///
    [[nodiscard]]
    bool is_surface(glm::uvec3 coords) const
    {
        return is_surface(to_offset(coords));
    }

    ///
    /// @brief Get the color of the block on the given coordinates
    ///
    /// @param coords Coordinates of the block
    /// @return Block color in ARGB format
    ///
    [[nodiscard]]
    std::uint32_t get_color(glm::uvec3 coords) const
    {
        return get_color(to_offset(coords));
    }

    ///
    /// @brief Set the color of the block on the given coordinates
    ///
    /// @param coords Coordinates of the block
    /// @param value Block color in ARGB format
    ///
    void set_color(glm::uvec3 coords, std::uint32_t value)
    {
        set_color(to_offset(coords), value);
    }

    ///
    /// @brief Get the height at which the highest-placed block is located
    ///
    /// @param coords Coordinates of the block
    /// @return The z-coordinate of the block
    ///
    [[nodiscard]]
    std::uint32_t get_height(glm::uvec2 coords) const;

    ///
    /// @brief Get the height at which the highest-placed block is located
    ///
    /// @param coords Coordinates of the block
    /// @return The z-coordinate of the block
    ///
    [[nodiscard]]
    float get_height_f(glm::vec2 coords) const;

    ///
    /// @brief Read single column from memory
    ///
    /// @param data Data
    /// @param offset Offset
    /// @return Data starting with next column or end
    ///
    std::span<std::byte> read_column_from_memory(std::span<std::byte> data, std::uint32_t& offset);

    ///
    /// @brief Load map from memory (VXL format)
    ///
    /// @param data Beginning of the memory region
    ///
    void read_from_memory(std::span<std::byte> data);

    ///
    /// @brief Load map from file (VXL format)
    ///
    /// @param filepath Path to the file (VXL format file)
    ///
    // void read_from_file(std::string_view filepath)
    // {
    //     boost::iostreams::mapped_file_source source(std::string { filepath });
    //     if (!source.is_open())
    //     {
    //         throw std::runtime_error("failed to open map file");
    //     }

    //     const auto * data = reinterpret_cast<const std::uint8_t *>(source.data());
    //     read_from_memory(data);
    // }

    ///
    /// @brief Write single column from offset to memory
    ///
    /// @param result Result
    /// @param offset Offset
    ///
    void write_column_to_memory(std::vector<std::uint8_t>& result, std::uint32_t& offset);

    ///
    /// @brief Write map to memory (to VXL format)
    ///
    /// @param result Output (vector)
    ///
    void write_to_memory(std::vector<std::uint8_t>& result);

    ///
    /// @brief Write map to memory and compress
    ///
    /// @param result Result buffer
    /// @todo Rewrite write function to stream
    ///
    // void write_and_compress(std::vector<char> & result)
    // {
    //     std::vector<std::uint8_t> data;
    //     write_to_memory(data);

    //     result.clear();
    //     result.reserve(std::size_t(512 * 512));

    //     try
    //     {
    //         auto *                         data_in = reinterpret_cast<char *>(data.data());
    //         bio::stream<bio::array_source> in(data_in, data.size());

    //         bio::filtering_streambuf<bio::input> filter;
    //         filter.push(boost::iostreams::zlib_compressor());
    //         filter.push(in);

    //         bio::back_insert_device<std::vector<char>>              out_device(result);
    //         bio::stream<bio::back_insert_device<std::vector<char>>> out(out_device);

    //         bio::copy(filter, out);
    //     }
    //     catch (const bio::zlib_error & ex)
    //     {
    //         auto error = ex.error();
    //         std::cerr << "zlib error: " << error << std::endl;
    //     }
    //     catch (...)
    //     {
    //         std::cerr << "unknown zlib error" << std::endl;
    //     }

    //     mChanged = false;
    // }

private:

    std::array<Block, SIZE_XYZ> m_blocks{};
    bool                        m_changed{false};
};

} // namespace cxxserver
