/**
 *
 * SpadesServer
 *
 */

#include "Map.hxx"

#include <cstring>
#include <iostream>

// width = 512
// height = 512
// depth = 64
//      0 = top
//      63 = bottom
//
// (0,0) -> (511,0)
// next (0,1)
//
// z = 63 -> (under) water - solid
// z = 62 -> indestructible - solid
//
// column consist of spans (heights from z=0 to z=63)
// span:
//  0 or more -> open voxels
//  1 or more -> colored voxels (top)
//  0 or more -> solid voxels
//  0 or more -> colored voxels (bottom)
//
// span format:
//      0   N   length of span data (N * 4 including span header)
//      1   S   starting height of top colored run
//      2   E   ending height of top colored run (length = E - S + 1)
//      3   A   starting height of air run (first span ignores value and assumes A=0)
//  4+i*4   b   blue color for colored #i
//  5+i*4   g   green color for colored #i
//  6+i*4   r   red color for colored #i
//  7+i*4   a   alpha channel for colored #i, actually used for shading in unknown way
//
//  i: 0 -> N - 1
//
// heights for the bottom colors are implied by the starting height of the air run of the next span; they appear
// just above the air
//
// next byte after the above list (at 8 + N * 4) is the first byte of the next span of the column
//
// location of the air run of the next span is at (8 + N*4 + 3)
//
// if N = 0 -> last span of the column
// last span contains air, a top-colored span, and all voxels below down to 63 are solid
//
// storage used by the span is determined by the number of top colors
// = 4 * (1 + (E - S + 1))
//
// K = E - S + 1
// Z = (N - 1) - K or 0 if N == 0
// M = A stored in next span or 64 if last span of the column
//
//  Run 	Start 	End 	    Length
//  air 	A 	    S - 1 	    S - A
//  top 	S 	    E 	        E - S + 1
//  solid   E + 1   M - Z - 1   M - Z - (E + 1)
//  bottom  M - Z   M - 1       Z
//

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
                for (; z <= endTop; ++z, ++color) {
                    SetColor(x, y, z, *color);
                }

                uint8 lengthBottom = endTop - startTop + 1;

                if (spanSize == 0) {
                    data += 4 * (lengthBottom + 1);
                    // std::cout << z << ' ';
                    // if (x == 511) {
                    //     std::cout << '\n';
                    // }
                    break;
                }

                uint8 lengthTop = (spanSize - 1) - lengthBottom;

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
            for (uint32 z = 0; z < 64;) {

                // air
                for (; z < 64 && !IsBlock(x, y, z); ++z) {
                }

                // top block
                startTop = z;
                for (; z < 64 && IsSurface(x, y, z); ++z) {
                }
                endTop = z;

                // not visible blocks
                for (; z < 64 && !IsSurface(x, y, z); ++z) {
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

                    // std::cout << static_cast<int>(startTop) << " ";
                    // if (x == 511) {
                    //     std::cout << '\n';
                    // }
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
