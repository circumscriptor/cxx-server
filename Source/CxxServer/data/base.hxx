/**
 * @file base.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "entity.hxx"

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
    base_data(team_type team) noexcept
        : entity { entity_type::base, object_id::base_id(team), team }
    {
    }
};

} // namespace spadesx
