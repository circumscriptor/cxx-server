/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Enums.hxx"
#include "Types.hxx"
#include "Vector.hxx"

typedef struct _ENetPacket ENetPacket;

namespace Spades {

class DataStream
{
  public:
    explicit DataStream(ENetPacket* packet);

    uint32 Left() const
    {
        return (mPos > mLength) ? 0 : mLength - mPos;
    }

    void Skip(uint32 skip)
    {
        mPos = (mPos + skip > mLength) ? mLength : mPos + skip;
    }

    template<typename T>
    T Read()
    {
        return static_cast<T>(mData[mPos++]);
    }

    uint8 ReadByte()
    {
        return mData[mPos++];
    }

    uint16 ReadShort()
    {
        uint16 value = 0;
        value |= static_cast<uint16>(mData[mPos++]);
        value |= static_cast<uint16>(mData[mPos++]) << 8;
        return value;
    }

    uint32 ReadInt()
    {
        uint32 value = 0;
        value |= static_cast<uint32>(mData[mPos++]);
        value |= static_cast<uint32>(mData[mPos++]) << 8;
        value |= static_cast<uint32>(mData[mPos++]) << 16;
        value |= static_cast<uint32>(mData[mPos++]) << 24;
        return value;
    }

    float ReadFloat()
    {
        uint32 value = ReadInt();
        return *reinterpret_cast<float*>(&value);
    }

    Vector3b ReadVector3b()
    {
        return {ReadByte(), ReadByte(), ReadByte()};
    }

    Vector3f ReadVector3f()
    {
        return {ReadFloat(), ReadFloat(), ReadFloat()};
    }

    void ReadArray(void* buffer, uint32 length);

    template<typename T>
    void Write(T type)
    {
        mData[mPos++] = static_cast<uint8>(type);
    }

    void WriteByte(uint8 value)
    {
        mData[mPos++] = value;
    }

    void WriteShort(uint16 value)
    {
        mData[mPos++] = static_cast<uint8>(value);
        mData[mPos++] = static_cast<uint8>(value >> 8);
    }

    void WriteInt(uint32 value)
    {
        mData[mPos++] = static_cast<uint8>(value);
        mData[mPos++] = static_cast<uint8>(value >> 8);
        mData[mPos++] = static_cast<uint8>(value >> 16);
        mData[mPos++] = static_cast<uint8>(value >> 24);
    }

    void WriteFloat(float value)
    {
        WriteInt(*reinterpret_cast<uint32*>(&value));
    }

    void WriteVector3b(const Vector3b& value)
    {
        WriteByte(value.x);
        WriteByte(value.y);
        WriteByte(value.z);
    }

    void WriteVector3f(const Vector3f& value)
    {
        WriteFloat(value.x);
        WriteFloat(value.y);
        WriteFloat(value.z);
    }

    void WriteArray(const void* data, uint32 length);

  private:
    uint8* mData;
    uint32 mLength;
    uint32 mPos;
};

} // namespace Spades
