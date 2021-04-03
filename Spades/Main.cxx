/**
 *
 * SpadesServer
 *
 */

#include "Server.hxx"

#include <iostream>

int main()
{
    std::cout << "Spades Server\n";
    return Spades::Server().Run(10);
}
