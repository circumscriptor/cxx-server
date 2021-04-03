/**
 *
 * SpadesServer
 *
 */

#include "Server.hxx"

#include <iostream>

// TODO: Use events, like HitEvent, ChatEvent, ...

int main()
{
    std::cout << "Spades Server\n";
    return Spades::Server().Run(10);
}
