/**
 * @file datastream.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "data/color.hxx"
#include "enet.hxx"

#include <cassert>
#include <cstring>
#include <string_view>

namespace spadesx {

/**
 * @brief Helper class for filling and reading packets
 *
 */
class data_stream
{
  public:

    /**
     * @brief Construct a new data_stream object
     *
     * @param packet ENetPacket pointer
     */
    data_stream(ENetPacket * packet) noexcept
        : m_data { packet->data }
        , m_end { packet->data + packet->dataLength }
    {
    }

    /**
     * @brief Construct a new data_stream object
     *
     * @param data_start Data start
     * @param data_end Data end
     */
    data_stream(std::uint8_t * data_start, std::uint8_t * data_end) noexcept
        : m_data { data_start }
        , m_end { data_end }
    {
    }

    /**
     * @brief Construct a new data_stream object
     *
     * @tparam N Length of the source in bytes
     * @param source Source array
     */
    template <std::size_t N>
    data_stream(std::array<std::uint8_t, N> & source) noexcept
        : m_data { source.begin() }
        , m_end { source.end() }
    {
    }

    /**
     * @brief Construct a new data_stream object
     *
     * @param data Data
     * @param size Size
     */
    data_stream(std::uint8_t * data, std::size_t size) noexcept
        : m_data { data }
        , m_end { data + size }
    {
    }

    /**
     * @brief Construct a new data_stream object
     *
     * @param other Other data_stream object to be copied
     */
    data_stream(const data_stream & other) noexcept = default;

    /**
     * @brief Construct a new data_stream object
     *
     * @param other Other data_stream object to be moved
     */
    data_stream(data_stream && other) noexcept = default;

    /**
     * @brief Copy the contents of another data_stream object
     *
     * @param other Another data_stream object
     * @return Reference to this
     */
    data_stream & operator=(const data_stream & other) = default;

    /**
     * @brief Current position
     *
     * @return Pointer
     */
    [[nodiscard]] const char * data() const noexcept
    {
        return reinterpret_cast<const char *>(m_data);
    }

    // /**
    //  * @brief Current position
    //  *
    //  * @return Pointer
    //  */
    // [[nodiscard]] std::uint8_t* begin() const noexcept
    // {
    //     return m_data;
    // }

    // /**
    //  * @brief End
    //  *
    //  * @return Pointer
    //  */
    // [[nodiscard]] std::uint8_t* end() const noexcept
    // {
    //     return m_end;
    // }

    /**
     * @brief Get number of bytes left (length - position)
     *
     * @return Number of bytes left
     */
    [[nodiscard]] std::size_t left() const noexcept
    {
        return m_data > m_end ? 0 : m_end - m_data;
    }

    /**
     * @brief Skip n-bytes
     *
     * @param skip Number of bytes to skip
     */
    void skip(std::size_t skip)
    {
        assert(m_data + skip <= m_end);
        m_data = m_data + skip;
    }

    /**
     * @brief Read 8-bit word
     *
     * @return 8-bit word
     */
    std::uint8_t read_byte()
    {
        assert(m_data + 1 <= m_end);
        return *m_data++;
    }

    /**
     * @brief Read 16-bit word
     *
     * @return 16-bit word
     */
    std::uint16_t read_short()
    {
        assert(m_data + 2 <= m_end);
        std::uint16_t value = 0;
        value               |= static_cast<std::uint16_t>(*m_data++);
        value               |= static_cast<std::uint16_t>(*m_data++) << 8;
        return value;
    }

    /**
     * @brief Read 32-bit word
     *
     * @return 32-bit word
     */
    std::uint32_t read_int()
    {
        assert(m_data + 4 <= m_end);
        std::uint32_t value = 0;
        value               |= static_cast<std::uint32_t>(*m_data++);
        value               |= static_cast<std::uint32_t>(*m_data++) << 8;
        value               |= static_cast<std::uint32_t>(*m_data++) << 16;
        value               |= static_cast<std::uint32_t>(*m_data++) << 24;
        return value;
    }

    /**
     * @brief Read 8-bit word (convert to type)
     *
     * @tparam T The type to convert to
     * @return Type
     */
    template <typename T>
    T read_type()
    {
        return static_cast<T>(read_byte());
    }

    /**
     * @brief Read 32-bit word (bitcast to float)
     *
     * @return Float number
     */
    float read_float()
    {
        std::uint32_t value = read_int();
        return *reinterpret_cast<float *>(&value);
    }

    /**
     * @brief Read RGB color (BGR order)
     *
     * @param result Color3 object
     */
    void read_color3b(color3b & result)
    {
        result.b = read_byte();
        result.g = read_byte();
        result.r = read_byte();
    }

    /**
     * @brief Read 3D-vector
     *
     * @param result Vector
     */
    void read_vec3(glm::vec3 & result)
    {
        result.x = read_float();
        result.y = read_float();
        result.z = read_float();
    }

    /**
     * @brief Read integer 3D-vector
     *
     * @param result Vector
     */
    void read_ivec3(glm::ivec3 & result)
    {
        result.x = static_cast<int>(read_int());
        result.y = static_cast<int>(read_int());
        result.z = static_cast<int>(read_int());
    }

    /**
     * @brief Read integer 3D-vector
     *
     * @param result Vector
     */
    void read_uvec3(glm::uvec3 & result)
    {
        result.x = read_int();
        result.y = read_int();
        result.z = read_int();
    }

    /**
     * @brief Read n-bytes and store in the buffer
     *
     * @param buffer The buffer to be used
     * @param length Amount of bytes to read
     */
    void read_array(void * buffer, std::size_t length)
    {
        assert(m_data + length <= m_end);
        std::memcpy(buffer, m_data, length);
        m_data += length;
    }

    /**
     * @brief Write 8-bit word
     *
     * @param value 8-bit word
     */
    void write_byte(std::uint8_t value)
    {
        assert(m_data + 1 <= m_end);
        *m_data++ = value;
    }

    /**
     * @brief Write 16-bit word
     *
     * @param value 16-bit word
     */
    void write_short(std::uint16_t value)
    {
        assert(m_data + 2 <= m_end);
        *m_data++ = static_cast<std::uint8_t>(value);
        *m_data++ = static_cast<std::uint8_t>(value >> 8);
    }

    /**
     * @brief Write 32-bit word
     *
     * @param value 32-bit word
     */
    void write_int(std::uint32_t value)
    {
        assert(m_data + 4 <= m_end);
        *m_data++ = static_cast<std::uint8_t>(value);
        *m_data++ = static_cast<std::uint8_t>(value >> 8);
        *m_data++ = static_cast<std::uint8_t>(value >> 16);
        *m_data++ = static_cast<std::uint8_t>(value >> 24);
    }

    /**
     * @brief Write type as 8-bit word
     *
     * @tparam T The type
     * @param value Value
     */
    template <typename T>
    void write_type(T value)
    {
        write_byte(static_cast<std::uint8_t>(value));
    }

    /**
     * @brief Write float number (bitcast to 32-bit word)
     *
     * @param value Float number
     */
    void write_float(float value)
    {
        write_int(*reinterpret_cast<std::uint32_t *>(&value));
    }

    /**
     * @brief Write RGB color (BGR order)
     *
     * @param value Color3 object
     */
    void write_color3b(const color3b & value)
    {
        write_byte(value.b);
        write_byte(value.g);
        write_byte(value.r);
    }

    /**
     * @brief Write 3D-vector
     *
     * @param value Vector
     */
    void write_vec3(const glm::vec3 & value)
    {
        write_float(value.x);
        write_float(value.y);
        write_float(value.z);
    }

    /**
     * @brief Write integer 3D-vector
     *
     * @param value Vector
     */
    void write_ivec3(const glm::ivec3 & value)
    {
        write_int(value.x);
        write_int(value.y);
        write_int(value.z);
    }

    /**
     * @brief Write unsigned 3D-vector
     *
     * @param value Vector
     */
    void write_uvec3(const glm::uvec3 & value)
    {
        write_int(value.x);
        write_int(value.y);
        write_int(value.z);
    }

    /**
     * @brief Write n-bytes
     *
     * @param buffer The buffer to be used
     * @param length Amount of bytes to write
     */
    void write_array(const void * buffer, std::uint32_t length)
    {
        assert(m_data + length <= m_end);
        std::memcpy(m_data, buffer, length);
        m_data += length;
    }

  private:

    std::uint8_t * m_data;
    std::uint8_t * m_end;
};

} // namespace spadesx
