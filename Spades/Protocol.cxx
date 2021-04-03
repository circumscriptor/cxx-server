#include "Protocol.hxx"

#include "Compress.hxx"
#include "Player.hxx"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <enet/enet.h>
#include <fstream>
#include <iostream>
#include <new>

Spades::Protocol::Protocol(uint8 maxPlayers) :
    mCompressor(5),
    mMaxPlayers{maxPlayers},
    mTeams{{"TEAM A", 6}, {"TEAM B", 6}}
{
    if (mMaxPlayers > 32) {
        mMaxPlayers = 32;
    }
    mPlayers = reinterpret_cast<Player*>(std::malloc(sizeof(Player) * 32));
    for (uint8 i = 0; i < 32; ++i) {
        new (mPlayers + i) Player(i, this);
        mPlayers[i].mID = i;
    }
}

Spades::Protocol::~Protocol()
{
    delete[] mPlayers;
}

Spades::Vector3f Spades::Protocol::GetSpawnLocation(Team::TeamE team)
{
    switch (team) {
        case Team::A:
        case Team::B:
        {
            uint8    t = static_cast<uint8>(team);
            Vector3f v;
            v.x = mTeams[t].spawn.a.x + (float(std::rand()) / float(RAND_MAX)) * mTeams[t].spawn.b.x;
            v.y = mTeams[t].spawn.a.y + (float(std::rand()) / float(RAND_MAX)) * mTeams[t].spawn.b.y;
            v.z = 62.f;
            return v;
        } break;
        default:
            return {256.f, 256.f, 0.f};
    }
}

void Spades::Protocol::LoadMap(const char* path)
{
    std::fstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to open file: " << path << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size];
    file.read(buffer, size);

    std::cout << "map loaded, map size: " << size << "\ncompressing map\n";

    mChunks = mCompressor.Compress(buffer, size, 8192);

    delete[] buffer;
}

void Spades::Protocol::TryConnect(ENetPeer* peer)
{
    if (!peer) {
        return;
    }
    std::cout << "trying connect player: " << peer->address.host << ':' << peer->address.port << '\n';
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (mPlayers[i].GetState() == State::Disconnected) {
            if (!mPlayers[i].OnConnect(peer)) {
                mPlayers[i].Reset();
                return;
            }
            std::cout << "player id " << ((int) i) << '\n';
            peer->data = mPlayers + i;
            mNumPlayers++;
            break;
        }
    }
    std::cout << "connected\n";
}

void Spades::Protocol::NotifyCreatePlayer(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendCreatePlayer(player);
        }
    }
}

void Spades::Protocol::NotifyPlayerLeft(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendPlayerLeft(player);
        }
    }
}

void Spades::Protocol::TryDisconnect(ENetPeer* peer)
{
    if (!peer || !peer->data) {
        return;
    }
    Player* player = reinterpret_cast<Player*>(peer->data);
    peer->data     = nullptr;
    NotifyPlayerLeft(*player);
    player->Reset();
}

void Spades::Protocol::ProcessInput(Player& player, DataStream& stream)
{
    PacketType type = stream.Read<PacketType>();
    switch (type) {
        case PacketType::PositionData:
            player.ReadPosition(stream);
            break;
        case PacketType::OrientationData:
            player.ReadOrientation(stream);
            break;
        case PacketType::ExistingPlayer:
            if (player.ReadExistingPlayer(stream)) {
                player.mState = State::Spawning;
            }
            break;
        default:
            std::cout << "unhandled packet " << static_cast<uint8>(type) << '\n';
            break;
    }
}

void Spades::Protocol::BroadcastWorldUpdate()
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendWorldUpdate();
        }
    }
}

void Spades::Protocol::Setup()
{
    mTeams[0].base = GetSpawnLocation(Team::A);
    mTeams[1].base = GetSpawnLocation(Team::B);

    mTeams[0].intel = GetSpawnLocation(Team::A);
    mTeams[1].intel = GetSpawnLocation(Team::B);

    mPreviousTime = std::chrono::steady_clock::now();
}

void Spades::Protocol::UpdatePlayer(Player& player)
{
    switch (player.GetState()) {
        case State::StartingMap:
        {
            std::cout << "sending start map packet\n";
            DataChunk* chunk = mChunks; // TODO: Implement
            player.StartSendingMap(chunk);
        } break;
        case State::LoadingChunks:
        {
            std::cout << "sending map chunk packet\n";
            player.ContinueSendingMap();
        } break;
        case State::Joining:
        {
            std::cout << "sending state data packet\n";
            if (player.SendStateData()) {
                player.mState = State::Waiting;
            }
            for (uint8 i = 0; i < mMaxPlayers; ++i) {
                if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
                    player.SendCreatePlayer(mPlayers[i]);
                }
            }
        } break;
        case State::Waiting:
        {
            // std::cout << "waiting for respawn\n";
        } break;
        case State::Spawning:
        {
            std::cout << "spawning player\n";
            player.mPosition = GetSpawnLocation(player.mTeam);
            NotifyCreatePlayer(player);
            player.mState = State::Ready;
        } break;
        case State::Ready:
        {
            // ?
        } break;
        case State::Disconnected:
            break;
    }
}

void Spades::Protocol::Update()
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        UpdatePlayer(mPlayers[i]);
    }

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<double>(now - mPreviousTime).count() > 0.1) {
        mPreviousTime = now;
        BroadcastWorldUpdate();
    }
}
