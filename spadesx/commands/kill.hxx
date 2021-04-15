/**
 *
 * SpadesX
 *
 */

#pragma once

#include "../baseprotocol.hxx"

namespace spadesx {

class kill_command : public command
{
  public:
    void on_enable() override
    {
    }

    void on_disable() override
    {
    }

    void on_execute(std::string_view /*args*/, connection& connection, base_protocol& protocol) override
    {
        if (connection.m_alive) {
            protocol.kill_and_broadcast(connection, connection, kill_type::weapon, 1);
        } else {
            protocol.system_message(connection, "player is not alive");
        }
    }
};

} // namespace spadesx