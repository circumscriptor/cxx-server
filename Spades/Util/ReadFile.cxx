/**
 *
 * SpadesServer
 *
 */

#include "ReadFile.hxx"

void* Spades::ReadFile(const char* path, uint32& size)
{
    std::fstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to read file: " << path << '\n';
        return nullptr;
    }

    file.seekg(0, std::ios::end);
    auto length = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    size = length;
    file.close();
    return buffer;
}
