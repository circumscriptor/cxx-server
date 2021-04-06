/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace Spades {

/**
 * @brief Helper class
 *
 */
class File
{
  public:
    /**
     * @brief Read entire file into the buffer
     *
     * @param path The file path
     * @return A buffer filled with data (or empty on failure)
     */
    static auto Read(const std::string& path) -> std::vector<uint8>
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "failed to read file: " << path << '\n';
            return std::vector<uint8>();
        }

        file.unsetf(std::ios::skipws); // do not read new line characters

        file.seekg(0, std::ios::end);
        auto length = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8> buffer;
        std::copy(std::istream_iterator<uint8>(file), std::istream_iterator<uint8>(), std::back_inserter(buffer));
        return buffer;
    }
};

} // namespace Spades
