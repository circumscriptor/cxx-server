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

class DataStream
{
  public:
    DataStream(void* pointer, uint32 length, uint32 start = 0) noexcept :
        mData{reinterpret_cast<uint8*>(pointer)},
        mLength{length},
        mPosition{start}
    {
    }

    DataStream(const DataStream& other) : mData{other.mData}, mLength{other.mLength}, mPosition{other.mPosition}
    {
    }

    DataStream& operator=(const DataStream& other)
    {
        mData     = other.mData;
        mLength   = other.mLength;
        mPosition = other.mPosition;
        return *this;
    }

    void Reset()
    {
        mPosition = 0;
    }

    uint32 Left() const
    {
        return (mPosition > mLength) ? 0 : mLength - mPosition;
    }

    void Skip(uint32 skip)
    {
        assert(mPosition + skip <= mLength);
        mPosition = (mPosition + skip > mLength) ? mLength : mPosition + skip;
    }

    uint8 ReadByte()
    {
        assert(mPosition + 1 <= mLength);
        return mData[mPosition++];
    }

    uint16 ReadShort()
    {
        assert(mPosition + 2 <= mLength);
        uint16 value = 0;
        value |= static_cast<uint16>(mData[mPosition++]);
        value |= static_cast<uint16>(mData[mPosition++]) << 8;
        return value;
    }

    uint32 ReadInt()
    {
        assert(mPosition + 4 <= mLength);
        uint32 value = 0;
        value |= static_cast<uint32>(mData[mPosition++]);
        value |= static_cast<uint32>(mData[mPosition++]) << 8;
        value |= static_cast<uint32>(mData[mPosition++]) << 16;
        value |= static_cast<uint32>(mData[mPosition++]) << 24;
        return value;
    }

    template<typename T>
    T Read()
    {
        return static_cast<T>(ReadByte());
    }

    float ReadFloat()
    {
        uint32 value = ReadInt();
        return *reinterpret_cast<float*>(&value);
    }

    void ReadColor3b(Color3b& result)
    {
        result.b = ReadByte();
        result.g = ReadByte();
        result.b = ReadByte();
    }

    void ReadVector3f(Vector3f& result)
    {
        result.x = ReadFloat();
        result.y = ReadFloat();
        result.z = ReadFloat();
    }

    void ReadArray(void* buffer, uint32 length)
    {
        assert(mPosition + length <= mLength);
        std::memcpy(buffer, mData + mPosition, length);
    }

    void WriteByte(uint8 value)
    {
        assert(mPosition + 1 <= mLength);
        mData[mPosition++] = value;
    }

    void WriteShort(uint16 value)
    {
        assert(mPosition + 2 <= mLength);
        mData[mPosition++] = static_cast<uint8>(value);
        mData[mPosition++] = static_cast<uint8>(value >> 8);
    }

    void WriteInt(uint32 value)
    {
        assert(mPosition + 4 <= mLength);
        mData[mPosition++] = static_cast<uint8>(value);
        mData[mPosition++] = static_cast<uint8>(value >> 8);
        mData[mPosition++] = static_cast<uint8>(value >> 16);
        mData[mPosition++] = static_cast<uint8>(value >> 24);
    }

    template<typename T>
    void WriteType(T type)
    {
        WriteByte(static_cast<uint8>(type));
    }

    void WriteFloat(float value)
    {
        WriteInt(*reinterpret_cast<uint32*>(&value));
    }

    void WriteColor3b(const Color3b& value)
    {
        WriteByte(value.b);
        WriteByte(value.g);
        WriteByte(value.r);
    }

    void WriteVector3f(const Vector3f& value)
    {
        WriteFloat(value.x);
        WriteFloat(value.y);
        WriteFloat(value.z);
    }

    void WriteArray(const void* buffer, uint32 length)
    {
        assert(mPosition + length <= mLength);
        std::memcpy(mData + mPosition, buffer, length);
    }

  private:
    uint8* mData;
    uint32 mLength;
    uint32 mPosition;
};

} // namespace Spades
