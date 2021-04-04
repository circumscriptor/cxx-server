/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"

#include <fstream>
#include <iostream>

namespace Spades {

void* ReadFile(const char* path, uint32& size);

} // namespace Spades
