/**
 * @file line.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "map.hxx"

#include <glm/common.hpp>
#include <glm/ext/vector_int3.hpp>

namespace spadesx {

/**
 * @brief Block line generator
 *
 */
class block_line
{
  public:
    static constexpr const std::uint32_t max_line_length = 50;                //!< Max length
    static constexpr const int           tmax_alt_value  = 0x3FFFFFFF / 1024; //!< TMAX Alt?

    /**
     * @brief Fill line with positions
     *
     * @param v1 Starting position
     * @param v2 End position
     * @return Number of blocks in line
     */
    std::uint32_t generate(const glm::ivec3& v1, const glm::ivec3& v2)
    {
        std::uint32_t count = 0;       // line length (in blocks)
        glm::ivec3    pos   = v1;      // current position
        glm::ivec3    dist  = v2 - v1; // distance
        glm::ivec3    step;            // step in direction
        glm::ivec3    tmax;
        glm::ivec3    delta;

        step.x = dist.x < 0 ? -1 : 1;
        step.y = dist.y < 0 ? -1 : 1;
        step.z = dist.z < 0 ? -1 : 1;

        glm::ivec3 a = glm::abs(dist);

        if (a.x >= a.y && a.x >= a.z) {
            tmax.x = 512;
            tmax.y = a.y != 0 ? a.x * 512 / a.y : tmax_alt_value;
            tmax.z = a.z != 0 ? a.x * 512 / a.z : tmax_alt_value;
        } else if (a.y >= a.z) {
            tmax.x = a.x != 0 ? a.y * 512 / a.x : tmax_alt_value;
            tmax.y = 512;
            tmax.z = a.z != 0 ? a.y * 512 / a.z : tmax_alt_value;
        } else {
            tmax.x = a.x != 0 ? a.z * 512 / a.x : tmax_alt_value;
            tmax.y = a.y != 0 ? a.z * 512 / a.y : tmax_alt_value;
            tmax.z = 512;
        }

        delta.x = tmax.x * 2;
        delta.y = tmax.y * 2;
        delta.z = tmax.z * 2;

        while (true) {
            m_line[count++] = pos;

            if (count >= max_line_length || pos == v2) { // reached limit or end
                break;
            }

            if (tmax.z <= tmax.x && tmax.z <= tmax.y) {
                pos.z += step.z;
                if (pos.z >= int(map::size_z)) {
                    break;
                }
                tmax.z += delta.z;
            } else if (tmax.x < tmax.y) {
                pos.x += step.x;
                if (pos.x >= int(map::size_x)) {
                    break;
                }
                tmax.x += delta.x;
            } else {
                pos.y += step.y;
                if (pos.y >= int(map::size_y)) {
                    break;
                }
                tmax.y += delta.y;
            }
        }

        return count;
    }

    /**
     * @brief Get block coordinates
     *
     * @param index Block index (line)
     * @return Reference to coordinates
     */
    [[nodiscard]] const glm::ivec3& get(std::uint32_t index) const
    {
        assert(index < max_line_length);
        return m_line[index];
    }

  private:
    glm::ivec3 m_line[max_line_length];
};

} // namespace spadesx
