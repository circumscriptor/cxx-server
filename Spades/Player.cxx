/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Player.hxx"

#include "DataStream.hxx"
#include "Protocol.hxx"

#include <enet/enet.h>

void Spades::Player::OnConnect(ENetPeer* peer)
{
    if (peer->eventData != static_cast<uint32>(Version::v0_75)) {
        enet_peer_disconnect(peer, static_cast<uint32>(Reason::WrongProtocolVersion));
        return;
    }
    if (mProtocol->Available() == 0) {
        enet_peer_disconnect(peer, static_cast<uint32>(Reason::ServerFull));
        return;
    }
    // check max connections per ip
    mState = State::StartingMap;
}

void Spades::Player::SendMapData()
{
    ENetPacket* packet = enet_packet_create(NULL, 5, ENET_PACKET_FLAG_RELIABLE);
    DataStream  stream(packet);
    stream.Write(Packet::MapStart);
    stream.WriteInt(10000);

    if (enet_peer_send(mPeer, 0, packet) == 0) {
        mState = State::LoadingChunks;
    }
}

void Spades::Player::SendMapChunk()
{
    uint8* mapChunk  = nullptr; // get map chunk
    uint32 chunkSize = 0;       // get chunk size

    if (mapChunk == nullptr) {
        mState = State::Joining;
        return;
    }

    ENetPacket* packet = enet_packet_create(NULL, chunkSize + 1, ENET_PACKET_FLAG_RELIABLE);
    DataStream  stream(packet);
    stream.Write(Packet::MapChunk);
    stream.WriteArray(reinterpret_cast<char*>(mapChunk), chunkSize);

    if (enet_peer_send(mPeer, 0, packet) != 0) {
        // repeat chunk
    }
}

void Spades::Player::SendJoiningData()
{
    ENetPacket* packet = enet_packet_create(NULL, 104, ENET_PACKET_FLAG_RELIABLE);
    DataStream  stream(packet);
    stream.Write(Packet::StateData);
    stream.WriteByte(mID);
    stream.WriteVector3b(mProtocol->mFogColor);
    stream.WriteVector3b(mProtocol->mTeams[0].color);
    stream.WriteVector3b(mProtocol->mTeams[1].color);
    stream.WriteArray(mProtocol->mTeams[0].name, 10);
    stream.WriteArray(mProtocol->mTeams[1].name, 10);
    stream.WriteByte(mProtocol->mTeams[0].score);
    stream.WriteByte(mProtocol->mTeams[1].score);
    stream.WriteByte(mProtocol->mScoreLimit);
    stream.WriteByte(mProtocol->IntelFlags());

    if (!mProtocol->mTeams[0].taken) {
        stream.WriteVector3f(mProtocol->mTeams[0].intel);
    } else {
        stream.WriteByte(mProtocol->mTeams[0].holder);
        stream.Skip(11);
    }

    if (!mProtocol->mTeams[1].taken) {
        stream.WriteVector3f(mProtocol->mTeams[1].intel);
    } else {
        stream.WriteByte(mProtocol->mTeams[1].holder);
        stream.Skip(11);
    }

    stream.WriteVector3f(mProtocol->mTeams[0].base);
    stream.WriteVector3f(mProtocol->mTeams[1].base);

    if (enet_peer_send(mPeer, 0, packet) == 0) {
        mState = State::Spawning;
    }
}
