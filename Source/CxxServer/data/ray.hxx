/**
 * @file ray.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "../map.hxx"

namespace spadesx {

/**
 * @brief Auxiliary class for line intersection calculation
 *
 */
class ray
{
  public:

    /**
     * @brief Check for clear visibility between two points
     *
     * @param map Map
     * @param a First point
     * @param b Second point
     * @return true If there is no intersection
     */
    static bool intersects(const map & map, const glm::vec3 & a, const glm::vec3 & b)
    {
        glm::ivec3 v0;
        glm::ivec3 v1;
        glm::ivec3 v2;
        glm::vec3  v3;
        glm::vec3  v4;

        int count     = 0;

        v0            = a - 0.5F;
        v1            = b - 0.5F;

        count         += interesects_helper_func(v0.x, v1.x, v2.x, v3.x, v4.x, a.x, b.x);
        count         += interesects_helper_func(v0.y, v1.y, v2.y, v3.y, v4.y, a.y, b.y);
        count         += interesects_helper_func(v0.z, v1.z, v2.z, v3.z, v4.z, a.z, b.z);

        glm::ivec3 v5 = glm::vec3 { v3.x * v4.z - v3.z * v4.x, v3.y * v4.z - v3.z * v4.y, v3.y * v4.x - v3.x * v4.y };
        glm::ivec3 v6 = v4;

        for (count = std::min(count, 128); count != 0; --count)
        {
            if (((v5.x | v5.y) >= 0) && (v0.z != v1.z))
            {
                v0.z += v2.z;
                v5.x -= v6.x;
                v5.y -= v6.y;
            }
            else if ((v5.z >= 0) && (v0.x != v1.x))
            {
                v0.x += v2.x;
                v5.x += v6.z;
                v5.z -= v6.y;
            }
            else
            {
                v0.y += v2.y;
                v5.y += v6.z;
                v5.z += v6.x;
            }

            if (map.is_block_wrap(v0.x, v0.y, v0.z))
            {
                return true;
            }
        }
        return false;
    }

  private:

    // Helper function
    static int interesects_helper_func(int v0, int v1, int & v2, float & v3, float & v4, float a, float b)
    {
        if (v1 < v0)
        {
            v2 = -1;
            v3 = a - float(v0);
            v4 = (a - b) * 1024;
            return v0 - v1;
        }
        if (v1 != v0)
        {
            v2 = 1;
            v3 = float(v0) + 1 - a;
            v4 = (b - a) * 1024;
            return v1 - v0;
        }
        v3 = v4 = 0;
        return 0;
    }
};

} // namespace spadesx
