/**
 *
 * SpadesX
 *
 */

#pragma once

#include "data/entity.hxx"

namespace spadesx {

/**
 * @brief Base data
 *
 */
class base_data : public entity
{
  public:
    /**
     * @brief Construct a new base object
     *
     * @param team Team
     */
    constexpr base_data(team_type team) noexcept : entity{entity_type::base, object_id::base_id(team), team}
    {
    }
};

} // namespace spadesx
