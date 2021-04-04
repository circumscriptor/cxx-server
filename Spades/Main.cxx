/**
 *
 * SpadesServer
 *
 */

#include "Protocol.hxx"
#include "Server.hxx"

#include <iostream>

int main()
{
    Spades::Protocol protocol(32);
    return Spades::Server().Run(protocol, 1);
}
