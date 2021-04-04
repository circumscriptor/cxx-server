/**
 *
 * SpadesServer
 *
 */

#include "Protocol.hxx"

#include <enet/enet.h>
#include <iostream>

Spades::Protocol::Protocol(uint8 maxPlayers) : mMaxPlayers(maxPlayers), mCompressor(5)
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
        case PacketType::ExistingPlayer:
        {
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

            connection.mCanSpawn = true;
            // connection.mWaitingForSpawn = true;
        } break;
        default:
            std::cout << "unhandled code: " << static_cast<int>(type) << '\n';
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

            auto* chunk          = mCompressor.Compress(mapData.data(), mapData.size(), 8192);
            connection.mMapChunk = chunk;

            // get map size

            uint32 mapSize = 0;
            while (chunk) {
                mapSize += chunk->mLength;
                chunk = chunk->mNext;
            }

            DataStream packet(mCache.mMapStart, sizeof(mCache.mMapStart));
            packet.WriteType(PacketType::MapChunk);
            packet.WriteInt(mapSize);
            connection.Send(packet);
            connection.mMapStart = false;
        } else {
            if (connection.mMapChunk == nullptr) {
                std::cout << "sending map state\n";
                //
                // Add existing players
                //
                for (auto& other : mConnections) {
                    if (connection != other && other) {
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
                //
                // Create players
                //
                for (auto& other : mConnections) {
                    if (connection != other && other && other.mAlive) {
                        DataStream packet(mCache.mCreatePlayer, sizeof(mCache.mCreatePlayer));
                        packet.WriteType(PacketType::ExistingPlayer);
                        packet.WriteByte(other.mID);
                        packet.WriteType(other.mWeapon);
                        packet.WriteType(other.mTeam);
                        packet.WriteVector3f(other.mPosition);
                        packet.WriteArray(other.mName, 16);
                        connection.Send(packet);
                    }
                }
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
                packet.WriteByte(mTeams[0].mScore);
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
            } else {
                std::cout << "sending map chunk\n";
                DataStream packet(connection.mMapChunk->mChunk, connection.mMapChunk->mLength);
                if (connection.Send(packet)) {
                    connection.mMapChunk = connection.mMapChunk->Pop();
                }
            }
        }
    } else if (connection.mState == State::Connected) {
        if (connection.mCanSpawn && !connection.mAlive) {
            connection.mAlive = true;
            // if (connection.mCanSpawn && connection.mWaitingForSpawn && connection.mRespawnTime == 0) {
            connection.mPosition = Vector3f(); // GetRespawnPosition(team);

            DataStream packet(mCache.mCreatePlayer, sizeof(mCache.mCreatePlayer));
            packet.WriteType(PacketType::CreatePlayer);
            packet.WriteByte(connection.mID);
            packet.WriteType(connection.mWeapon);
            packet.WriteType(connection.mTeam);
            packet.WriteVector3f(connection.mPosition);
            packet.WriteArray(connection.mName, 16);
            Broadcast(connection, packet, true);
        }
    }
}

void Spades::Protocol::Update()
{
    for (auto& connection : mConnections) {
        if (connection) {
            UpdateConnection(connection);
        }
    }
}
