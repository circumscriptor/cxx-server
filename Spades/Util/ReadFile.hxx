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

/**
 * @brief Read entire file into the buffer
 *
 * @param path The file path
 * @param size Ouput size
 * @return A buffer filled with data or nullptr on failure
 */
auto ReadFile(const char* path, uint32& size) -> void*;

} // namespace Spades
