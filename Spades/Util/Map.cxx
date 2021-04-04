/**
 *
 * SpadesServer
 *
 */

#include "Map.hxx"

#include <cstring>
#include <iostream>

bool Spades::Map::Load(uint8* data, uint32 length)
{
    if (!data) {
        return false;
    }

    std::memset(mBlocks, 0xFF, ((((1 << 9) << 9) << 6) >> 5) << 2);

    uint8   spanSize, startTop, endTop, lengthTop, startAir, endBottom, startBottom, lengthBottom;
    uint32* color;

    for (uint32 y = 0; y < 512; ++y) {
        for (uint32 x = 0; x < 512; ++x) {
            for (uint32 z = 0;;) {
                spanSize = data[0]; // N
                startTop = data[1]; // S
                endTop   = data[2]; // E
                startAir = data[3]; // A

                for (; z < startTop; ++z) {
                    SetBlock(x, y, z, false);
                }

                color = (uint32*) (data + 4);
                for (z = startTop; z <= endTop; ++z, ++color) {
                    SetColor(x, y, z, *color);
                }

                lengthBottom = endTop - startTop + 1;

                if (spanSize == 0) {
                    data += 4 * (lengthBottom + 1);
                    break;
                }

                lengthTop = (spanSize - 1) - lengthBottom;

                data += spanSize * 4;

                endBottom   = data[3]; // next start air
                startBottom = endBottom - lengthTop;

                for (z = startBottom; z < endBottom; ++z, ++color) {
                    SetColor(x, y, z, *color);
                }
            }
        }
    }
    return true;
}

void Spades::Map::Save(std::vector<uint8>& output)
{
    output.clear();
    output.reserve(512 * 512 * 8);
    uint8 airStart, startTop, endTop, startBottom, endBottom, lengthTop, lengthBottom, lengthColors;
    for (uint32 y = 0; y < 512; ++y) {
        for (uint32 x = 0; x < 512; ++x) {
            uint8 r = 0;
            for (uint32 z = 0; z < 64;) {
                // air
                airStart = z;
                for (; z < 64 && !IsBlock(x, y, z); ++z) {
                }

                // top
                startTop = z;
                for (; z < 64 && IsSurface(x, y, z); ++z) {
                }
                endTop = z;

                // not visible blocks
                for (; z < 64 && IsBlock(x, y, z) && !IsSurface(x, y, z); ++z) {
                }

                // bottom?
                uint32 t    = z;
                startBottom = z;

                for (; t < 64 && IsSurface(x, y, t); ++t) {
                }

                if (t != 64) {
                    for (; IsSurface(x, y, z); ++z) {
                    }
                }
                endBottom = z;

                lengthTop    = endTop - startTop;
                lengthBottom = endBottom - startBottom;
                lengthColors = lengthTop + lengthBottom;

                if (z == 64) {
                    output.push_back(0); // last span in column
                } else {
                    output.push_back(lengthColors + 1);
                }

                output.push_back(startTop);
                output.push_back(endTop - 1);
                output.push_back(airStart);

                for (uint32 i = 0; i < lengthTop; ++i) {
                    uint32 color = GetColor(x, y, startTop + i);
                    output.push_back(color & 0xFF);
                    output.push_back((color >> 8) & 0xFF);
                    output.push_back((color >> 16) & 0xFF);
                    output.push_back((color >> 24) & 0xFF);
                }

                for (uint32 i = 0; i < lengthBottom; ++i) {
                    uint32 color = GetColor(x, y, startBottom + i);
                    output.push_back(color & 0xFF);
                    output.push_back((color >> 8) & 0xFF);
                    output.push_back((color >> 16) & 0xFF);
                    output.push_back((color >> 24) & 0xFF);
                }
            }
        }
    }
}
