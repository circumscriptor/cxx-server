/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Types.hxx"
#include "Vector.hxx"

#include <cassert>
#include <cstring>

namespace Spades {

/**
 * @brief Helper class for filling and reading packets
 *
 */
class DataStream
{
  public:
    /**
     * @brief Construct a new DataStream object
     *
     * @param pointer Pointer to the memory block
     * @param length Length of the memory block
     * @param start Starting position
     */
    DataStream(void* pointer, uint32 length, uint32 start = 0) noexcept :
        mData{reinterpret_cast<uint8*>(pointer)},
        mLength{length},
        mPosition{start}
    {
    }

    /**
     * @brief Construct a new DataStream object
     *
     * @param other Other DataStream object to be copied
     */
    DataStream(const DataStream& other) noexcept = default;

    // Default copy assignment
    auto operator=(const DataStream& other) -> DataStream& = default;

    /**
     * @brief Reset position
     *
     */
    void Reset() noexcept
    {
        mPosition = 0;
    }

    /**
     * @brief Get number of bytes left (length - position)
     *
     * @return uint32
     */
    [[nodiscard]] auto Left() const -> uint32
    {
        return (mPosition > mLength) ? 0 : mLength - mPosition;
    }

    /**
     * @brief Skip n-bytes
     *
     * @param skip Number of bytes to skip
     */
    void Skip(uint32 skip)
    {
        assert(mPosition + skip <= mLength);
        mPosition = (mPosition + skip > mLength) ? mLength : mPosition + skip;
    }

    /**
     * @brief Read 8-bit word
     *
     * @return 8-bit word
     */
    auto ReadByte() -> uint8
    {
        assert(mPosition + 1 <= mLength);
        return mData[mPosition++];
    }

    /**
     * @brief Read 16-bit word
     *
     * @return 16-bit word
     */
    auto ReadShort() -> uint16
    {
        assert(mPosition + 2 <= mLength);
        uint16 value = 0;
        value |= static_cast<uint16>(mData[mPosition++]);
        value |= static_cast<uint16>(mData[mPosition++]) << 8;
        return value;
    }

    /**
     * @brief Read 32-bit word
     *
     * @return 32-bit word
     */
    auto ReadInt() -> uint32
    {
        assert(mPosition + 4 <= mLength);
        uint32 value = 0;
        value |= static_cast<uint32>(mData[mPosition++]);
        value |= static_cast<uint32>(mData[mPosition++]) << 8;
        value |= static_cast<uint32>(mData[mPosition++]) << 16;
        value |= static_cast<uint32>(mData[mPosition++]) << 24;
        return value;
    }

    /**
     * @brief Read 8-bit word (convert to type)
     *
     * @tparam T The type to convert to
     * @return Type
     */
    template<typename T>
    auto ReadType() -> T
    {
        return static_cast<T>(ReadByte());
    }

    /**
     * @brief Read 32-bit word (bitcast to float)
     *
     * @return Float number
     */
    auto ReadFloat() -> float
    {
        uint32 value = ReadInt();
        return *reinterpret_cast<float*>(&value);
    }

    /**
     * @brief Read RGB color (BGR order)
     *
     * @param result Color3 object
     */
    void ReadColor3(Color3& result)
    {
        result.b = ReadByte();
        result.g = ReadByte();
        result.r = ReadByte();
    }

    /**
     * @brief Read 3D-vector
     *
     * @param result Vector3 object
     */
    void ReadVector3(Vector3& result)
    {
        result.x = ReadFloat();
        result.y = ReadFloat();
        result.z = ReadFloat();
    }

    /**
     * @brief Read n-bytes and store in the buffer
     *
     * @param buffer The buffer to be used
     * @param length Amount of bytes to read
     */
    void ReadArray(void* buffer, uint32 length)
    {
        assert(mPosition + length <= mLength);
        std::memcpy(buffer, mData + mPosition, length);
        mPosition += length;
    }

    /**
     * @brief Write 8-bit word
     *
     * @param value 8-bit word
     */
    void WriteByte(uint8 value)
    {
        assert(mPosition + 1 <= mLength);
        mData[mPosition++] = value;
    }

    /**
     * @brief Write 16-bit word
     *
     * @param value 16-bit word
     */
    void WriteShort(uint16 value)
    {
        assert(mPosition + 2 <= mLength);
        mData[mPosition++] = static_cast<uint8>(value);
        mData[mPosition++] = static_cast<uint8>(value >> 8);
    }

    /**
     * @brief Write 32-bit word
     *
     * @param value 32-bit word
     */
    void WriteInt(uint32 value)
    {
        assert(mPosition + 4 <= mLength);
        mData[mPosition++] = static_cast<uint8>(value);
        mData[mPosition++] = static_cast<uint8>(value >> 8);
        mData[mPosition++] = static_cast<uint8>(value >> 16);
        mData[mPosition++] = static_cast<uint8>(value >> 24);
    }

    /**
     * @brief Write type as 8-bit word
     *
     * @tparam T The type
     * @param value Value
     */
    template<typename T>
    void WriteType(T value)
    {
        WriteByte(static_cast<uint8>(value));
    }

    /**
     * @brief Write float number (bitcast to 32-bit word)
     *
     * @param value Float number
     */
    void WriteFloat(float value)
    {
        WriteInt(*reinterpret_cast<uint32*>(&value));
    }

    /**
     * @brief Write RGB color (BGR order)
     *
     * @param value Color3 object
     */
    void WriteColor3(const Color3& value)
    {
        WriteByte(value.b);
        WriteByte(value.g);
        WriteByte(value.r);
    }

    /**
     * @brief Write 3D-vector
     *
     * @param value Vector3 object
     */
    void WriteVector3(const Vector3& value)
    {
        WriteFloat(value.x);
        WriteFloat(value.y);
        WriteFloat(value.z);
    }

    /**
     * @brief Write n-bytes
     *
     * @param buffer The buffer to be used
     * @param length Amount of bytes to write
     */
    void WriteArray(const void* buffer, uint32 length)
    {
        assert(mPosition + length <= mLength);
        std::memcpy(mData + mPosition, buffer, length);
        mPosition += length;
    }

    /**
     * @brief Get stored pointer
     *
     * @return Pointer
     */
    [[nodiscard]] auto Data() const noexcept -> void*
    {
        return reinterpret_cast<void*>(mData);
    }

    /**
     * @brief Get stored length
     *
     * @return Length
     */
    [[nodiscard]] auto Length() const noexcept -> uint32
    {
        return mLength;
    }

  private:
    uint8* mData;
    uint32 mLength;
    uint32 mPosition;
};

} // namespace Spades
