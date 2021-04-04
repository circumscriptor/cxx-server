/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"

typedef struct _ENetPeer   ENetPeer;
typedef struct _ENetPacket ENetPacket;

namespace Spades {

class Protocol
{
  public:
    /**
     * @brief Construct a new Protocol object
     *
     */
    Protocol() : Protocol(32)
    {
    }

    /**
     * @brief Construct a new Protocol object
     *
     * @param maxPlayers Max number of players
     */
    Protocol(uint8 maxPlayers);

    /**
     * @brief Destroy the Protocol object
     *
     */
    ~Protocol();

    void TryConnect(ENetPeer* peer);

    void TryDisconnect(ENetPeer* peer);

    void Receive(ENetPeer* peer, ENetPacket* packet);

    void Update();

  private:
};

} // namespace Spades
