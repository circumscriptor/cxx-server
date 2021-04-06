/**
 *
 * SpadesServer
 *
 */

#include "Protocol.hxx"

#include "Core/DataStream.hxx"
#include "Core/Vector.hxx"
#include "Data/Enums.hxx"

#include <cstring>
#include <enet/enet.h>
#include <iostream>

Spades::Protocol::Protocol(uint8 maxPlayers) : mMaxPlayers(maxPlayers)
{
    for (uint8 i = 0; i < 32; ++i) {
        mConnections.emplace_back(i);
    }
}

Spades::Protocol::~Protocol()
{
}

void Spades::Protocol::Broadcast(const Connection& sender, DataStream& data, bool includeSender, uint8 channel)
{
    for (auto& connection : mConnections) {
        if (!includeSender && sender == connection) {
            continue;
        }
        if (connection) {
            connection.Send(data, channel);
        }
    }
}

void Spades::Protocol::OnConnect(ENetPeer* peer)
{
    std::cout << "trying to connect...\n";
    if (!peer) {
        return;
    }
    if (peer->eventData != static_cast<uint32>(Version::v0_75)) {
        enet_peer_disconnect(peer, static_cast<uint32>(Reason::WrongProtocolVersion));
        return;
    }
    if (mNumPlayers == mMaxPlayers) {
        enet_peer_disconnect(peer, static_cast<uint32>(Reason::ServerFull));
        return;
    }
    for (auto& connection : mConnections) {
        if (!bool(connection)) {
            std::cout << "assigning connection\n";
            connection.Connect(peer);
            peer->data = &connection;
            mNumPlayers++;
            return;
        }
    }
}

void Spades::Protocol::OnDisconnect(ENetPeer* peer)
{
    if (!peer || !peer->data) {
        return;
    }
    Connection* connection = reinterpret_cast<Connection*>(peer->data);
    peer->data             = nullptr;

    // create packet
    DataStream packet(mCache.mPlayerLeft, sizeof(mCache.mPlayerLeft), 0);
    packet.WriteType(PacketType::PlayerLeft);
    packet.WriteByte(connection->mID);
    Broadcast(*connection, packet, false);

    // cleanup
    connection->Reset();
    mNumPlayers--;
}

void Spades::Protocol::Receive(ENetPeer* peer, ENetPacket* packet)
{
    Connection& connection = *reinterpret_cast<Connection*>(peer->data);
    DataStream  stream(packet->data, packet->dataLength, 0);
    PacketType  type = stream.ReadType<PacketType>();

    switch (type) {
        case PacketType::PositionData:
            stream.ReadVector3f(connection.mPosition);
            break;
        case PacketType::OrientationData:
            stream.ReadVector3f(connection.mOrientation);
            break;
        case PacketType::InputData:
        {
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "input data, invalid id received\n";
            }
            uint8 input        = stream.ReadByte();
            connection.mUp     = input & 0x01;
            connection.mDown   = input & 0x02;
            connection.mLeft   = input & 0x04;
            connection.mRight  = input & 0x08;
            connection.mJump   = input & 0x10;
            connection.mCrouch = input & 0x20;
            connection.mSneak  = input & 0x40;
            connection.mSprint = input & 0x80;
            Broadcast(connection, stream, false);
        } break;
        case PacketType::WeaponInput:
        {
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "weapon input, invalid id received\n";
            }
            uint8 input           = stream.ReadByte();
            connection.mPrimary   = input & 0x1;
            connection.mSecondary = input & 0x02;
            Broadcast(connection, stream, false);
        } break;
        case PacketType::SetTool:
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "set tool, invalid id received\n";
            }

            connection.mTool = stream.ReadType<Tool>();
            Broadcast(connection, stream, false);
            break;
        case PacketType::SetColor:
        {
            auto id = stream.ReadByte();
            if (connection.mID != id) {
                // something wrong
                std::cout << "set color, invalid id received: " << static_cast<int>(id) << '\n';
                // weird bug - changes colors of other players
            }

            stream.ReadColor3b(connection.mColor);
            Broadcast(connection, stream, false);
        } break;
        case PacketType::ExistingPlayer:
        {
            std::cout << "existing player from: " << static_cast<int>(connection.mID) << '\n';
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "existing player, invalid id received\n";
            }
            connection.mTeam   = stream.ReadType<TeamType>();
            connection.mWeapon = stream.ReadType<Weapon>();
            connection.mTool   = stream.ReadType<Tool>();
            connection.mKills  = stream.ReadInt();
            stream.ReadColor3b(connection.mColor);

            auto length = stream.Left();
            length      = (length > 16) ? 16 : length;
            stream.ReadArray(connection.mName, length);
            connection.mName[length] = 0;

            if (connection.mAlive) {
                std::cout << "WARNING: this is not suppossed to happen\n";
                connection.mAlive = false;
            }

            connection.mCanSpawn = true;
            // connection.mWaitingForSpawn = true;
        } break;
        case PacketType::BlockAction:
        {
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "change team, invalid id received\n";
            }
            // TODO: Add ability to refuse block action
            auto type = stream.ReadType<BlockActionType>();
            auto x    = stream.ReadInt();
            auto y    = stream.ReadInt();
            auto z    = stream.ReadInt();

            if (type == BlockActionType::BulletOrSpade) {
                mMap.SetBlock(x, y, z, false);
            } else if (type == BlockActionType::Build) {
                mMap.SetBlock(x, y, z, true);
                mMap.SetColor(x, y, z, ColorToU32(connection.mColor));
            }

            Block(connection, x, y, z, type);
        } break;
        case PacketType::ChangeTeam:
        {
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "change team, invalid id received\n";
            }
            // TODO: Add ability to refuse team change
            connection.mTeam = stream.ReadType<TeamType>();

            if (connection.mAlive) {
                connection.mRespawnTime = mRespawnTime;
                Kill(connection, connection, KillType::TeamChange, mRespawnTime);
            }
        } break;
        case PacketType::ChangeWeapon:
        {
            if (connection.mID != stream.ReadByte()) {
                // something wrong
                std::cout << "change weapon, invalid id received\n";
            }
            // TODO: Add ability to refuse weapon change
            connection.mWeapon = stream.ReadType<Weapon>();

            if (connection.mAlive) {
                connection.mRespawnTime = mRespawnTime;
                Kill(connection, connection, KillType::WeaponChange, mRespawnTime);
            }
        } break;
        default:
            std::cout << "from " << static_cast<int>(connection.mID) << " code: " << static_cast<int>(type) << '\n';
            break;
    }
}

void Spades::Protocol::UpdateConnection(Connection& connection)
{
    if (connection.mState == State::Connecting) {
        if (connection.mMapStart) {
            std::cout << "starting map send\n";
            std::vector<uint8> mapData;
            mMap.Save(mapData);

            std::cout << "map raw length: " << mapData.size() << '\n';

            auto* chunk          = Compressor::Compress(5, mapData.data(), mapData.size(), 8192);
            connection.mMapChunk = chunk;

            // get map size

            uint32 mapSize = 0;
            while (chunk) {
                mapSize += chunk->mLength;
                chunk = chunk->mNext;
            }

            DataStream packet(mCache.mMapStart, sizeof(mCache.mMapStart));
            packet.WriteType(PacketType::MapStart);
            packet.WriteInt(mapSize);
            connection.Send(packet);
            connection.mMapStart = false;
        } else {
            if (connection.mMapChunk == nullptr) {
                //
                // Add existing players
                //
                for (auto& other : mConnections) {
                    if (connection != other && other.mState == State::Connected) { // send only connected players
                        std::cout << "sending existing player: " << static_cast<int>(other.mID) << '\n';
                        DataStream packet(mCache.mExistingPlayer, sizeof(mCache.mExistingPlayer));
                        packet.WriteType(PacketType::ExistingPlayer);
                        packet.WriteByte(other.mID);
                        packet.WriteType(other.mTeam);
                        packet.WriteType(other.mWeapon);
                        packet.WriteType(other.mTool);
                        packet.WriteInt(other.mKills);
                        packet.WriteColor3b(other.mColor);
                        packet.WriteArray(other.mName, 16);
                        connection.Send(packet);
                    }
                }
                for (auto& other : mConnections) {
                    if (connection != other && other.mState == State::Connected && !other.mAlive) {
                        std::cout << "sending kill packet: " << static_cast<int>(other.mID) << '\n';
                        DataStream packet(mCache.mKillAction, sizeof(mCache.mExistingPlayer));
                        packet.WriteType(PacketType::KillAction);
                        packet.WriteByte(other.mID);
                        packet.WriteByte(other.mLastKillKiller);
                        packet.WriteType(other.mLastKillType);
                        packet.WriteByte(other.mRespawnTime);
                        connection.Send(packet);
                    }
                }
                std::cout << "sending map state\n";
                //
                // Send state data
                //
                DataStream packet(mCache.mStateData, sizeof(mCache.mStateData));
                packet.WriteType(PacketType::StateData);
                packet.WriteByte(connection.mID);
                packet.WriteColor3b(mFogColor);
                packet.WriteColor3b(mTeams[0].mColor);
                packet.WriteColor3b(mTeams[1].mColor);
                packet.WriteArray(mTeams[0].mName, 10);
                packet.WriteArray(mTeams[1].mName, 10);
                packet.WriteType(Mode::CTF);
                // CTF
                packet.WriteByte(mTeams[0].mScore + 9);
                packet.WriteByte(mTeams[1].mScore);
                packet.WriteByte(mScoreLimit);
                packet.WriteByte(IntelFlags());
                if (!mTeams[0].mIntelTaken) {
                    packet.WriteVector3f(mTeams[0].mIntel);
                } else {
                    packet.WriteByte(mTeams[0].mIntelHolder);
                    packet.Skip(11);
                }
                if (!mTeams[1].mIntelTaken) {
                    packet.WriteVector3f(mTeams[1].mIntel);
                } else {
                    packet.WriteByte(mTeams[1].mIntelHolder);
                    packet.Skip(11);
                }
                packet.WriteVector3f(mTeams[0].mBase);
                packet.WriteVector3f(mTeams[1].mBase);
                if (connection.Send(packet)) {
                    connection.mState = State::Connected;
                }
                //
                // Create players
                //
                // for (auto& other : mConnections) {
                //     if (connection != other && other.mAlive) {
                //         std::cout << "sending create player: " << static_cast<int>(other.mID) << '\n';
                //         DataStream packet(mCache.mCreatePlayer, sizeof(mCache.mCreatePlayer));
                //         packet.WriteType(PacketType::CreatePlayer);
                //         packet.WriteByte(other.mID);
                //         packet.WriteType(other.mWeapon);
                //         packet.WriteType(other.mTeam);
                //         packet.WriteVector3f(other.mPosition);
                //         packet.WriteArray(other.mName, 16);
                //         connection.Send(packet);
                //     }
                // }
            } else {
                // std::cout << "sending map chunk\n";
                DataStream packet(connection.mMapChunk->mChunk, connection.mMapChunk->mLength);
                if (connection.Send(packet)) {
                    connection.mMapChunk = connection.mMapChunk->Pop();
                }
            }
        }
    } else if (connection.mState == State::Connected) {
        if (connection.mCanSpawn && !connection.mAlive && connection.mRespawnTime == 0) {
            connection.mAlive = true;
            // if (connection.mCanSpawn && connection.mWaitingForSpawn && connection.mRespawnTime == 0) {

            GetSpawnLocation(connection.mTeam, connection.mPosition);
            Create(connection);
        }
    }
}

void Spades::Protocol::Kill(Connection& killer, Connection& victim, KillType type, uint8 respawnTime)
{
    victim.mAlive          = false;
    victim.mLastKillKiller = killer.mID;
    victim.mLastKillType   = type;
    victim.mRespawnTime    = respawnTime;

    DataStream packet(mCache.mKillAction, sizeof(mCache.mKillAction));
    packet.WriteType(PacketType::KillAction);
    packet.WriteByte(victim.mID);
    packet.WriteByte(victim.mLastKillKiller);
    packet.WriteType(victim.mLastKillType);
    packet.WriteByte(victim.mRespawnTime);
    Broadcast(killer, packet, true);
}

void Spades::Protocol::Create(Connection& connection)
{
    DataStream packet(mCache.mCreatePlayer, sizeof(mCache.mCreatePlayer));
    packet.WriteType(PacketType::CreatePlayer);
    packet.WriteByte(connection.mID);
    packet.WriteType(connection.mWeapon);
    packet.WriteType(connection.mTeam);
    packet.WriteVector3f(connection.mPosition);
    packet.WriteArray(connection.mName, 16);
    Broadcast(connection, packet, true);
}

void Spades::Protocol::Block(Connection& connection, uint32 x, uint32 y, uint32 z, BlockActionType action)
{
    DataStream packet(mCache.mBlockAction, sizeof(mCache.mBlockAction));
    packet.WriteType(PacketType::BlockAction);
    packet.WriteByte(connection.mID);
    packet.WriteType(action);
    packet.WriteInt(x);
    packet.WriteInt(y);
    packet.WriteInt(z);
    Broadcast(connection, packet, true);
}

void Spades::Protocol::Start()
{
    GetSpawnLocation(TeamType::A, mTeams[0].mBase);
    GetSpawnLocation(TeamType::B, mTeams[1].mBase);
    GetSpawnLocation(TeamType::A, mTeams[0].mIntel);
    GetSpawnLocation(TeamType::B, mTeams[1].mIntel);
    mRespawnTime = 2; // TODO: Move
}

void Spades::Protocol::Update()
{
    for (auto& connection : mConnections) {
        if (connection) {
            UpdateConnection(connection);
        }
    }

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<double>(now - mRespawnTimer).count() >= 1) {
        mRespawnTimer = now;
        for (auto& connection : mConnections) {
            if (connection.mRespawnTime != 0) {
                connection.mRespawnTime--;
            }
        }
    }

    if (std::chrono::duration<double>(now - mWorldUpdateTimer).count() >= 0.1) {
        mWorldUpdateTimer = now;
        DataStream stream(mCache.mWorldUpdate, sizeof(mCache.mWorldUpdate));
        stream.WriteType(PacketType::WorldUpdate);
        for (auto& connection : mConnections) {
            stream.WriteVector3f(connection.mPosition);
            stream.WriteVector3f(connection.mOrientation);
        }
        Broadcast(mConnections[0], stream, true); // [0] because functions needs any connection
    }
}
