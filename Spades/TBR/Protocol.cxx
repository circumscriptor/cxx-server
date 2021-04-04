#include "Protocol.hxx"

#include "Compress.hxx"
#include "Enums.hxx"
#include "Packet.hxx"
#include "Player.hxx"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <enet/enet.h>
#include <fstream>
#include <iostream>
#include <new>

Spades::Protocol::Protocol(uint8 maxPlayers) : mCompressor(5), mMaxPlayers{maxPlayers}
{
    if (mMaxPlayers > 32) {
        mMaxPlayers = 32;
    }
    for (uint8 i = 0; i < 32; ++i) {
        mPlayers[i].mID = i;
    }
}

Spades::Protocol::~Protocol()
{
}

Spades::Vector3f Spades::Protocol::GetSpawnLocation(Team::Enum team)
{
    switch (team) {
        case Team::A:
        case Team::B:
        {
            uint8    t = static_cast<uint8>(team);
            Vector3f v;
            // v.x = mTeams[t].mSpawn.a.x + (float(std::rand()) / float(RAND_MAX)) * mTeams[t].mSpawn.b.x;
            // v.y = mTeams[t].mSpawn.a.y + (float(std::rand()) / float(RAND_MAX)) * mTeams[t].mSpawn.b.y;
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

void Spades::Protocol::OnConnect(ENetPeer* peer)
{
    if (!peer) {
        return;
    }
    std::cout << "trying connect player: " << peer->address.host << ':' << peer->address.port << '\n';
    if (peer->eventData != static_cast<uint32>(Version::v0_75)) {
        enet_peer_disconnect(peer, static_cast<uint8>(Reason::WrongProtocolVersion));
        return;
    }
    if (Available() == 0) {
        enet_peer_disconnect(peer, static_cast<uint8>(Reason::ServerFull));
        return;
    }
    for (auto& player : mPlayers) {
        if (player) {
            if (!player.OnConnect(peer)) {
                player.Reset();
                return;
            }
            std::cout << "player id " << player.mID << '\n';
            peer->data = &player;
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
    for (auto& p : mPlayers) {
        if (p != player && p) {
            p.SendPlayerLeft(player);
        }
    }
}

void Spades::Protocol::NotifyPlayerInput(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendInputData(player);
        }
    }
}

void Spades::Protocol::NotifyWeaponInput(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendWeaponInput(player);
        }
    }
}

void Spades::Protocol::NotifySetTool(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendSetTool(player);
        }
    }
}

void Spades::Protocol::NotifySetColor(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendSetColor(player);
        }
    }
}

void Spades::Protocol::NotifyWeaponReload(const Player& player)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (i != player.mID && mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendWeaponReload(player);
        }
    }
}

void Spades::Protocol::NotifyKillAction(const Player& player, const Player& target, KillType type, uint8 respawnTime)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendKillAction(player, target, type, respawnTime);
        }
    }
}
void Spades::Protocol::NotifyGrenade(const GrenadeEvent& event)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (mPlayers[i].GetState() != State::Disconnected) {
            mPlayers[i].SendGrenadePacket(event);
        }
    }
}

void Spades::Protocol::NotifyChatMessage(const Player& player, const ChatEvent& event)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        if (mPlayers[i].GetState() != State::Disconnected) {
            if (event.type == ChatType::Team && player.mTeam != mPlayers[i].mTeam) {
                continue;
            }
            mPlayers[i].SendChatMessage(event);
        }
    }
}

void Spades::Protocol::OnDisconnect(ENetPeer* peer)
{
    if (!peer || !peer->data) {
        return;
    }
    Player* player = reinterpret_cast<Player*>(peer->data);
    peer->data     = nullptr;
    NotifyPlayerLeft(*player);
    player->Reset();
    mNumPlayers--;
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
        case PacketType::InputData:
            player.ReadInputData(stream);
            NotifyPlayerInput(player);
            break;
        case PacketType::WeaponInput:
            player.ReadWeaponInput(stream);
            NotifyWeaponInput(player);
            break;
        case PacketType::GrenadePacket:
        {
            GrenadeEvent event;
            player.ReadGrenadePacket(stream, event);
            NotifyGrenade(event);
        } break;
        case PacketType::SetTool:
            player.ReadSetTool(stream);
            NotifySetTool(player);
            break;
        case PacketType::SetColor:
            player.ReadSetColor(stream);
            NotifySetColor(player);
            break;
        case PacketType::HitPacket:
        {
            HitEvent hit = player.ReadHitPacket(stream);
            // check

            KillType kill;

            switch (hit.type) {
                case HitType::Torso:
                case HitType::Arms:
                case HitType::Legs:
                    kill = KillType::Weapon;
                    break;
                case HitType::Head:
                    kill = KillType::Headshot;
                    break;
                case HitType::Melee:
                    kill = KillType::Melee;
                    break;
            }

            Player& target = mPlayers[hit.target];
            std::cout << static_cast<int>(player.mID) << " kills " << static_cast<int>(target.mID) << '\n';

            NotifyKillAction(player, target, kill, 5);
            target.mRespawnTime = 5; // TODO: change
            target.mState       = State::Respawning;
        } break;
        case PacketType::ExistingPlayer:
            if (player.mState != State::Respawning) {
                if (player.ReadExistingPlayer(stream)) {
                    player.mState = State::Spawning;
                }
            }
            std::cout << "existing player data\n";
            break;
        case PacketType::ChatMessage:
        {

            ChatEvent event;
            player.ReadChatMessage(stream, event);
            NotifyChatMessage(player, event);
            std::cout << "message: " << event.message << '\n';
            delete[] event.message; // TODO: change
        } break;
        case PacketType::WeaponReload:
        {
            auto reserve = player.ReadWeaponReload(stream);
            // check reserve
            NotifyWeaponReload(player);
        } break;
        case PacketType::ChangeTeam:
        {
            std::cout << "team change request\n";
            auto team = player.ReadTeamChange(stream);
            // check whether it's possible
            player.mTeam = team;
            NotifyKillAction(player, player, KillType::TeamChange, 5);
            player.mState       = State::Respawning;
            player.mRespawnTime = 5;
        } break;
        default:
            std::cout << "unhandled packet " << static_cast<int>(type) << '\n';
            break;
    }
}

void Spades::Protocol::BroadcastWorldUpdate()
{
    // for (uint8 i = 0; i < mMaxPlayers; ++i) {
    //     if (mPlayers[i].GetState() != State::Disconnected) {
    //         mPlayers[i].SendWorldUpdate();
    //     }
    // }
}

void Spades::Protocol::Setup()
{
    // mTeams[0].mBase = GetSpawnLocation(Team::A);
    // mTeams[1].mBase = GetSpawnLocation(Team::B);

    // mTeams[0].mIntel = GetSpawnLocation(Team::A);
    // mTeams[1].mIntel = GetSpawnLocation(Team::B);

    // mWorldUpdateTimer = std::chrono::steady_clock::now();
    // mRespawnTimer     = mWorldUpdateTimer;
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
            // std::cout << "sending map chunk packet\n";
            player.ContinueSendingMap();
        } break;
        case State::Joining:
        {
            std::cout << "sending state data packet\n";
            for (uint8 i = 0; i < mMaxPlayers; ++i) {
                if (i != player.mID &&
                    (mPlayers[i].GetState() == State::Respawning || mPlayers[i].GetState() == State::Ready)) {
                    player.SendCreatePlayer(mPlayers[i]);
                }
            }
            if (player.SendStateData(mState)) {
                player.mState = State::Waiting;
            }
        } break;
        case State::Waiting:
        {
        } break;
        case State::Respawning:
        {
            // std::cout << "waiting for respawn\n";
            if (player.mRespawnTime == 0) {
                player.mState = State::Spawning;
            }
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

void Spades::Protocol::Update(double time)
{
    for (uint8 i = 0; i < mMaxPlayers; ++i) {
        UpdatePlayer(mPlayers[i]);
    }

    auto now = std::chrono::steady_clock::now();

    if (std::chrono::duration<double>(now - mRespawnTimer).count() >= 1) {
        mRespawnTimer = now;
        for (uint8 i = 0; i < mMaxPlayers; ++i) {
            if (mPlayers[i].GetState() != State::Disconnected && mPlayers[i].mRespawnTime != 0) {
                mPlayers[i].mRespawnTime--;
            }
        }
    }

    double delta = std::chrono::duration<double>(now - mWorldUpdateTimer).count();
    if (delta >= time) {
        mWorldUpdateTimer = now;
        for (uint8 i = 0; i < mMaxPlayers; ++i) {
            if (mPlayers[i].GetState() == State::Ready) {
                mPlayers[i].SetCrouch(mPlayers[i].mInput & 0x20);
            }
        }
        BroadcastWorldUpdate();
    }
}