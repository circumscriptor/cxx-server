/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Compress.hxx"
#include "DataStream.hxx"
#include "Enums.hxx"
#include "Packet.hxx"
#include "Peer.hxx"
#include "Team.hxx"
#include "Vector.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

struct StateData
{
    Vector3b mFogColor{0xff, 0xff, 0xff};
    Team     mTeams[2];
    uint8    mScoreLimit{10};

    StateData() = default;

    uint8 IntelFlags() const noexcept
    {
        uint8 flags = 0;
        flags |= (mTeams[0].mIntelTaken) ? 1 : 0;
        flags |= (mTeams[1].mIntelTaken) ? 2 : 0;
        return flags;
    }
};

struct WorldUpdate
{
    struct
    {
        Vector3f mPosition;
        Vector3f mOrientation;
    } mData[32];
};

struct Player
{
    friend class Protocol;

  public:
    /**
     * @brief Check whether is connected
     *
     * @return true If connected
     * @return false If disconnected
     */
    operator bool() const noexcept
    {
        return mState != State::Disconnected;
    }

    /**
     * @brief Reset all values
     *
     */
    void Reset() noexcept
    {
        mPeer.Set(nullptr);
        mPosition    = {0.f, 0.f, 0.f};
        mOrientation = {0.f, 0.f, 0.f};
        mState       = State::Disconnected;
        mInput       = 0;
        while (mMapChunk) {
            mMapChunk = mMapChunk->Pop();
        }
        mRespawnTime = 0;
    }

    /**
     * @brief Function executed on player connection
     *
     * @param peer Player connection
     * @return True on success
     */
    bool OnConnect(ENetPeer* peer)
    {
        mPeer.Set(peer);
        // check max connections per ip
        mState = State::StartingMap;
        return true;
    }

    /**
     * @brief Read position data
     *
     */
    void ReadPosition(DataStream& stream)
    {
        mPosition = stream.ReadVector3f();
    }

    /**
     * @brief Write position data
     *
     */
    void WritePosition()
    {
        Packet packet(1 + 12);
        packet.Write(PacketType::PositionData);
        packet.WriteVector3f(mPosition);
        mPeer.Send(packet);
    }

    /**
     * @brief Read orientation data
     *
     */
    void ReadOrientation(DataStream& stream)
    {
        mOrientation = stream.ReadVector3f();
    }

    void ReadInputData(DataStream& stream)
    {
        stream.ReadByte(); // ID
        mInput = stream.ReadByte();
    }

    void ReadWeaponInput(DataStream& stream)
    {
        stream.ReadByte(); // ID
        mWeaponInput = stream.ReadByte();
    }

    void ReadSetTool(DataStream& stream)
    {
        stream.ReadByte(); // ID
        mTool = stream.Read<Tool>();
    }

    void ReadSetColor(DataStream& stream)
    {
        stream.ReadByte(); // ID
        mColor = stream.ReadVector3b();
    }

    Team::Enum ReadTeamChange(DataStream& stream)
    {
        stream.ReadByte(); // ID
        return stream.Read<Team::Enum>();
    }

    HitEvent ReadHitPacket(DataStream& stream)
    {
        HitEvent event;
        event.player = mID;
        event.target = stream.ReadByte();
        event.type   = stream.Read<HitType>();
        return event;
    }

    uint8 ReadWeaponReload(DataStream& stream)
    {
        stream.ReadByte();
        mClipAmmo    = stream.ReadByte();
        mReserveAmmo = stream.ReadByte();
        return mReserveAmmo;
    }

    void SendInputData(const Player& player)
    {
        Packet packet(3);
        packet.Write(PacketType::InputData);
        packet.Write(player.mID);
        packet.Write(player.mInput);
        mPeer.Send(packet);
    }

    void SendWeaponInput(const Player& player)
    {
        Packet packet(3);
        packet.Write(PacketType::WeaponInput);
        packet.WriteByte(player.mID);
        packet.WriteByte(player.mWeaponInput);
        mPeer.Send(packet);
    }

    void SendSetTool(const Player& player)
    {
        Packet packet(3);
        packet.Write(PacketType::SetTool);
        packet.WriteByte(player.mID);
        packet.Write(player.mTool);
        mPeer.Send(packet);
    }

    void SendSetColor(const Player& player)
    {
        Packet packet(5);
        packet.Write(PacketType::SetColor);
        packet.Write(player.mID);
        packet.WriteVector3b(player.mColor);
        mPeer.Send(packet);
    }

    void SendWeaponReload(const Player& player)
    {
        Packet packet(5);
        packet.Write(PacketType::WeaponReload);
        packet.Write(player.mID);
        packet.Write(player.mClipAmmo);
        packet.Write(player.mReserveAmmo);
        mPeer.Send(packet);
    }

    void SendKillAction(const Player& player, const Player& target, KillType type, uint8 respawnTime)
    {
        Packet packet(5);
        packet.Write(PacketType::KillAction);
        packet.WriteByte(target.mID);
        packet.WriteByte(player.mID);
        packet.Write(type);
        packet.WriteByte(respawnTime);
        mPeer.Send(packet);
    }

    bool ReadExistingPlayer(DataStream& stream)
    {
        if (mID != stream.ReadByte()) {
            // something wrong
            return false;
        }
        mTeam   = stream.Read<Team::Enum>();
        mWeapon = stream.Read<Weapon>();
        mTool   = stream.Read<Tool>();
        mKills  = stream.ReadInt();
        mColor  = stream.ReadVector3b();

        auto length = stream.Left();
        length      = (length > 16) ? 16 : length;
        stream.ReadArray(mName, length);
        mName[length] = 0;
        return true;
    }

    void ReadShortPlayerData(DataStream& stream)
    {
        if (mID != stream.ReadByte()) {
            // something wrong
        }
        mTeam   = stream.Read<Team::Enum>();
        mWeapon = stream.Read<Weapon>();
    }

    void ReadGrenadePacket(DataStream& stream, GrenadeEvent& event)
    {
        if (mID != stream.ReadByte()) {
            // something wrong
        }
        event.player     = mID;
        event.fuseLength = stream.ReadFloat();
        event.position   = stream.ReadVector3f();
        event.velocity   = stream.ReadVector3f();
    }

    void ReadChatMessage(DataStream& stream, ChatEvent& event)
    {
        if (mID != stream.ReadByte()) {
            // something wrong
        }
        event.player = mID;
        event.type   = stream.Read<ChatType>();
        event.length = stream.Left();
        if (event.length > 255) {
            event.length = 255;
        }
        event.message               = new char[event.length + 1];
        event.message[event.length] = 0;
        stream.ReadArray(event.message, event.length);
    }

    void SendChatMessage(const ChatEvent& event)
    {
        Packet packet(3 + event.length);
        packet.Write(PacketType::ChatMessage);
        packet.WriteByte(event.player);
        packet.Write(event.type);
        packet.WriteArray(event.message, event.length);
        mPeer.Send(packet);
    }

    void SendGrenadePacket(const GrenadeEvent& event)
    {
        Packet packet(30);
        packet.Write(PacketType::GrenadePacket);
        packet.WriteByte(event.player);
        packet.WriteFloat(event.fuseLength);
        packet.WriteVector3f(event.position);
        packet.WriteVector3f(event.velocity);
        mPeer.Send(packet);
    }

    void SendCreatePlayer(const Player& other)
    {
        Packet packet(1 + 31);
        packet.Write(PacketType::CreatePlayer);
        packet.WriteByte(other.mID);
        packet.Write(other.mWeapon);
        packet.Write(other.mTeam);
        packet.WriteVector3f(other.mPosition);
        packet.WriteArray(other.mName, 16);
        mPeer.Send(packet);
    }

    void SendWorldUpdate(const WorldUpdate& update)
    {
        Packet packet(1 + 32 * 24, true);
        packet.Write(PacketType::WorldUpdate);
        for (uint8 i = 0; i < 32; ++i) {
            packet.WriteVector3f(update.mData[i].mPosition);
            packet.WriteVector3f(update.mData[i].mOrientation);
        }
        mPeer.Send(packet);
    }

    void SendPlayerLeft(const Player& other)
    {
        Packet packet(2);
        packet.Write(PacketType::PlayerLeft);
        packet.WriteByte(other.mID);
        mPeer.Send(packet);
    }

    /**
     * @brief Write position data
     *
     */
    void SendOrientation()
    {
        Packet packet(1 + 12);
        packet.Write(PacketType::OrientationData);
        packet.WriteVector3f(mPosition);
        mPeer.Send(packet);
    }

    /**
     * @brief Send map start packet
     *
     * @param chunk Map to be sent
     */
    void StartSendingMap(DataChunk* chunk)
    {
        if (chunk) {
            mMapChunk = chunk;

            uint32 mapSize = 0;
            while (chunk) {
                mapSize += chunk->mLength;
                chunk = chunk->mNext;
            }

            Packet packet(5);
            packet.Write(PacketType::MapStart);
            packet.WriteInt(mapSize);
            if (mPeer.Send(packet)) {
                mState = State::LoadingChunks;
            }
        }
    }

    /**
     * @brief Send map chunk
     *
     */
    void ContinueSendingMap()
    {
        if (mMapChunk == nullptr) {
            mState = State::Joining;
        } else {
            Packet packet(1 + mMapChunk->mLength);
            packet.Write(PacketType::MapChunk);
            packet.WriteArray(mMapChunk->mChunk, mMapChunk->mLength);
            if (mPeer.Send(packet)) {
                mMapChunk = mMapChunk->mNext;
            }
        }
    }

    /**
     * @brief Send state data (+ CTF state)
     *
     */
    bool SendStateData(const StateData& state)
    {
        Packet packet(1 + 31 + 52);

        packet.Write(PacketType::StateData);
        // 1
        packet.WriteByte(mID);
        packet.WriteVector3b(state.mFogColor);
        packet.WriteVector3b(state.mTeams[0].mColor);
        packet.WriteVector3b(state.mTeams[1].mColor);
        // 10
        packet.WriteArray(state.mTeams[0].mName, 10);
        packet.WriteArray(state.mTeams[1].mName, 10);
        // 20
        packet.Write(Mode::CTF);
        // 1

        // CTF
        packet.WriteByte(state.mTeams[0].mScore);
        packet.WriteByte(state.mTeams[1].mScore);
        packet.WriteByte(state.mScoreLimit);
        packet.WriteByte(state.IntelFlags());
        // 4

        if (!state.mTeams[0].mIntelTaken) {
            packet.WriteVector3f(state.mTeams[0].mIntel);
        } else {
            packet.WriteByte(state.mTeams[0].mIntelHolder);
            packet.Skip(11);
        }
        // 12

        if (!state.mTeams[1].mIntelTaken) {
            packet.WriteVector3f(state.mTeams[1].mIntel);
        } else {
            packet.WriteByte(state.mTeams[1].mIntelHolder);
            packet.Skip(11);
        }
        // 12

        packet.WriteVector3f(state.mTeams[0].mBase);
        packet.WriteVector3f(state.mTeams[1].mBase);
        // 24

        if (mPeer.Send(packet)) {
            return true;
        }
        return false;
    }

    State GetState() const noexcept
    {
        return mState;
    }

    void SetCrouch(bool value)
    {
        if (value != mCrouch) {
            if (value) {
                mPosition.z += 0.9f;
            } else {
                mPosition.z -= 0.9f;
            }
            mCrouch = value;
        }
    }

  private:
    Peer       mPeer;                       //!< Peer (connection)
    char       mName[17];                   //!< Player name
    uint32     mID;                         //!< Player ID
    Vector3f   mPosition;                   //!< Position
    Vector3f   mOrientation;                //!< Orientation
    Vector3f   mVelocity;                   //!< Player velocity
    State      mState{State::Disconnected}; //!< Player state
    Weapon     mWeapon;                     //!< Weapon
    Team::Enum mTeam;                       //!< Team
    Tool       mTool;                       //!< Currently held tool
    uint32     mKills{0};                   //!< Player kills
    Vector3b   mColor;                      //!< Block color
    uint8      mInput{0};                   //!< Input keys
    uint8      mWeaponInput{0};             //!< Weapon input (primary, secondary)
    DataChunk* mMapChunk{nullptr};          //!< Pointer to the next chunk to be sent
    uint8      mRespawnTime{0};             //!< Time to respawn
    uint8      mClipAmmo{0};
    uint8      mReserveAmmo{0};
    bool       mAirborne{false};
    bool       mCrouch{false};
};

} // namespace Spades