///
/// @file map.hxx
/// @brief Map
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <array>
#include <cassert>
#include <cstdint>

namespace cxxserver {

class Map
{
  public:

    struct Block
    {
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
    static const std::uint32_t SIZE_MASK_X     = SIZE_X - 1;       //!< Mask for horizontal coordinates
    static const std::uint32_t SIZE_MASK_Y     = SIZE_Y - 1;       //!< Mask for horizontal coordinates
    static const std::uint32_t SIZE_MASK_Z     = SIZE_Z - 1;       //!< Mask for vertical coordinates
    static const std::uint32_t SHIFT_X         = 9;
    static const std::uint32_t SHIFT_Y         = 9;
    static const std::uint32_t SHIFT_Z         = 6;

    ///
    /// @brief Construct a new map object
    ///
    ///
    Map() = default;

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
    [[nodiscard]] bool hasChanged() const noexcept
    {
        return mChanged;
    }

    ///
    /// @brief Modify single block
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @param value The new state of the block
    /// @param color Block color in ARGB format
    ///
    void modifyBlock(std::uint32_t x, std::uint32_t y, std::uint32_t z, bool value, std::uint32_t color);

    ///
    /// @brief Destroy single block
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    ///
    void destroyBlock(std::uint32_t x, std::uint32_t y, std::uint32_t z);

    ///
    /// @brief Destroy 3 block
    ///
    /// @param x The x-coordinate of the middle block
    /// @param y The y-coordinate of the middle block
    /// @param z The z-coordinate of the middle block
    ///
    void destroyBlockSecondary(std::uint32_t x, std::uint32_t y, std::uint32_t z);

    ///
    /// @brief Destroy up-to 27 blocks
    ///
    /// @param x The x-coordinate of the grenade explosion
    /// @param y The y-coordinate of the grenade explosion
    /// @param z The z-coordinate of the grenade explosion
    ///
    void destroyBlockGrenade(std::int32_t x, std::int32_t y, std::int32_t z);

    ///
    /// @brief Check whether the block at the given index is solid
    ///
    /// @param offset The offset
    /// @return true If the block is solid
    /// @return false If the block is not solid, i.e. is air
    ///
    [[nodiscard]] bool isSolid(std::uint32_t offset) const;

    ///
    /// @brief Set the state of the block on the given offset
    ///
    /// @param offset The offset
    /// @param value The new state of the block
    ///
    void setSolid(std::uint32_t offset, bool value);

    ///
    /// @brief Check whether the block is a surface block (is visible from at least one side)
    ///
    /// @param offset The offset
    /// @return true If the block is a surface block
    /// @return false If the block is not solid or if is not a surface block
    ///
    [[nodiscard]] bool isSurface(std::uint32_t offset) const;

    ///
    /// @brief Same as isSolid, but water is empty and out of bounds returns true
    ///
    /// @param x The x-coordinate of ...
    /// @param y The y-coordinate of ...
    /// @param z The z-coordinate of ...
    /// @return true ...
    ///
    [[nodiscard]] bool isClipBox(std::int32_t x, std::int32_t y, std::int32_t z) const;

    ///
    /// @brief Same as isSolid, but water is empty and out of bounds returns false
    ///
    /// @param x The x-coordinate of ...
    /// @param y The y-coordinate of ...
    /// @param z The z-coordinate of ...
    /// @return true ...
    ///
    [[nodiscard]] bool isClipWorld(std::int32_t x, std::int32_t y, std::int32_t z) const;

    ///
    /// @brief Same as is_block, but water is empty and out of bounds returns true
    ///
    /// @param x The x-coordinate of ...
    /// @param y The y-coordinate of ...
    /// @param z The z-coordinate of ...
    /// @return true ...
    ///
    [[nodiscard]] bool isClipBoxF(float x, float y, float z) const;

    ///
    /// @brief Check whether the block (or air) has a neighbor (another solid block attached)
    ///
    /// @param offset The offset
    /// @return true If the block has a neighbor
    /// @return false If the block does not have a neighbor
    ///
    [[nodiscard]] bool hasNeighbor(std::uint32_t offset) const;

    ///
    /// @brief Check whether the block (or air) has a neighbor (another solid block attached)
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @return true If the block has a neighbor
    /// @return false If the block does not have a neighbor
    ///
    [[nodiscard]] bool hasNeighbor(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return hasNeighbor(offset(x, y, z));
    }

    ///
    /// @brief Get the color of the block on the given offset
    ///
    /// @param offset The offset
    /// @return Block color in ARGB format
    ///
    [[nodiscard]] std::uint32_t getColor(std::uint32_t offset) const;

    ///
    /// @brief Set the color of the block on the given offset
    ///
    /// @param offset The offset
    /// @param value Block color in ARGB format
    ///
    void setColor(std::uint32_t offset, std::uint32_t value);

    ///
    /// @brief Calculate the offset of the block from the coordinates
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @return The offset of the block
    ///
    static constexpr std::uint32_t offset(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
    {
        return (((y << SHIFT_Y) + x) << SHIFT_Z) + z;
    }

    ///
    /// @brief Check whether the block at the given coordinates is solid
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @return true If the block is solid
    /// @return false If the block is not solid, i.e. is air
    ///
    [[nodiscard]] bool isSolid(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return isSolid(offset(x, y, z));
    }

    ///
    /// @brief Check whether the block at the given coordinates is solid (with wrapping)
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @return true If the block is solid
    /// @return false If the block is not solid, i.e. is air
    ///
    [[nodiscard]] bool isBlockWrap(std::int32_t x, std::int32_t y, std::int32_t z) const;

    ///
    /// @brief Set the state of the block on the given coordinates
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @param value The new state of the block
    ///
    void setSolid(std::uint32_t x, std::uint32_t y, std::uint32_t z, bool value);

    ///
    /// @brief Check whether the block is a surface block (is visible from at least one side)
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @return true If the block is a surface block
    /// @return false If the block is not solid or if is not a surface block
    ///
    [[nodiscard]] bool isSurface(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return isSurface(offset(x, y, z));
    }

    ///
    /// @brief Get the color of the block on the given coordinates
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @return Block color in ARGB format
    ///
    [[nodiscard]] std::uint32_t getColor(std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        return getColor(offset(x, y, z));
    }

    ///
    /// @brief Set the color of the block on the given coordinates
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @param z The z-coordinate of the block
    /// @param value Block color in ARGB format
    ///
    void set_color(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t value)
    {
        setColor(offset(x, y, z), value);
    }

    ///
    /// @brief Get the height at which the highest-placed block is located
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @return The z-coordinate of the block
    ///
    [[nodiscard]] std::uint32_t getHeight(std::uint32_t x, std::uint32_t y) const;

    ///
    /// @brief Get the height at which the highest-placed block is located
    ///
    /// @param x The x-coordinate of the block
    /// @param y The y-coordinate of the block
    /// @return The z-coordinate of the block
    ///
    [[nodiscard]] float getHeightF(float x, float y) const;

    ///
    /// @brief Read single column from memory to offset
    ///
    /// @param data Data
    /// @param offset Offset
    /// @return Next memory location
    ///
    const std::uint8_t * readColumnFromMemory(const std::uint8_t * data, std::uint32_t & offset);

    ///
    /// @brief Load map from memory (VXL format)
    ///
    /// @param data Beginning of the memory region
    ///
    void readFromMemory(const std::uint8_t * data);

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
    void writeColumnToMemory(std::vector<std::uint8_t> & result, std::uint32_t & offset);

    ///
    /// @brief Write map to memory (to VXL format)
    ///
    /// @param result Output (vector)
    ///
    void writeToMemory(std::vector<std::uint8_t> & result);

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

    std::array<Block, SIZE_XYZ> mBlocks {};
    bool                        mChanged { false };
};

} // namespace cxxserver
