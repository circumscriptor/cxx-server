/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Compress.hxx"
#include "DataStream.hxx"
#include "Enums.hxx"
#include "Peer.hxx"
#include "Protocol.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

struct HitEvent
{
    uint8   player;
    uint8   target;
    HitType type;
};

class Player
{
    friend class Protocol;

  public:
    /**
     * @brief Construct a new Player object
     *
     */
    Player(uint8 id, Protocol* protocol) : mID{id}, mProtocol{protocol}
    {
    }

    // Delete copy constructor
    Player(const Player&) = delete;

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
        if (mPeer.EventData() != static_cast<uint32>(Version::v0_75)) {
            mPeer.Disconnect(Reason::WrongProtocolVersion);
            return false;
        }
        if (mProtocol->Available() == 0) {
            mPeer.Disconnect(Reason::ServerFull);
            return false;
        }
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

    void SendWorldUpdate()
    {
        Packet packet(1 + 32 * 24);
        packet.Write(PacketType::WorldUpdate);
        for (uint8 i = 0; i < 32; ++i) {
            packet.WriteVector3f(mProtocol->mPlayers[i].mPosition);
            packet.WriteVector3f(mProtocol->mPlayers[i].mOrientation);
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
    bool SendStateData()
    {
        Packet packet(1 + 31 + 52);

        packet.Write(PacketType::StateData);
        // 1
        packet.WriteByte(mID);
        packet.WriteVector3b(mProtocol->mFogColor);
        packet.WriteVector3b(mProtocol->mTeams[0].color);
        packet.WriteVector3b(mProtocol->mTeams[1].color);
        // 10
        packet.WriteArray(mProtocol->mTeams[0].name, 10);
        packet.WriteArray(mProtocol->mTeams[1].name, 10);
        // 20
        packet.Write(mProtocol->mGameMode);
        // 1

        // CTF
        packet.WriteByte(mProtocol->mTeams[0].score);
        packet.WriteByte(mProtocol->mTeams[1].score);
        packet.WriteByte(mProtocol->mScoreLimit);
        packet.WriteByte(mProtocol->IntelFlags());
        // 4

        if (!mProtocol->mTeams[0].taken) {
            packet.WriteVector3f(mProtocol->mTeams[0].intel);
        } else {
            packet.WriteByte(mProtocol->mTeams[0].holder);
            packet.Skip(11);
        }
        // 12

        if (!mProtocol->mTeams[1].taken) {
            packet.WriteVector3f(mProtocol->mTeams[1].intel);
        } else {
            packet.WriteByte(mProtocol->mTeams[1].holder);
            packet.Skip(11);
        }
        // 12

        packet.WriteVector3f(mProtocol->mTeams[0].base);
        packet.WriteVector3f(mProtocol->mTeams[1].base);
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

  private:
    Peer       mPeer;                       //!< Peer (connection)
    char       mName[17];                   //!< Player name
    uint32     mID;                         //!< Player ID
    Vector3f   mPosition;                   //!< Position
    Vector3f   mOrientation;                //!< Orientation
    State      mState{State::Disconnected}; //!< Player state
    Weapon     mWeapon;                     //!< Weapon
    Team::Enum mTeam;                       //!< Team
    Tool       mTool;                       //!< Currently held tool
    uint32     mKills{0};                   //!< Player kills
    Vector3b   mColor;                      //!< Block color
    uint8      mInput{0};                   //!< Input keys
    uint8      mWeaponInput{0};             //!< Weapon input (primary, secondary)
    Protocol*  mProtocol;                   //!< Common protocol
    DataChunk* mMapChunk{nullptr};          //!< Pointer to the next chunk to be sent
    uint8      mRespawnTime{0};             //!< Time to respawn
    uint8      mClipAmmo{0};
    uint8      mReserveAmmo{0};
};

} // namespace Spades
