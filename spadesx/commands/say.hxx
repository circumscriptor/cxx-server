/**
 *
 * SpadesX
 *
 */

#pragma once

#include "../baseprotocol.hxx"

namespace spadesx {

class say_command : public command
{
  public:
    void on_enable() override
    {
    }

    void on_disable() override
    {
    }

    void on_execute(std::string_view args, connection& connection, base_protocol& protocol) override
    {
        std::string message = "player ";
        message += connection.name();
        message += " says: ";
        message += args;
        protocol.broadcast_system_message(message);
    }
};

} // namespace spadesx
