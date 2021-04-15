/**
 *
 * SpadesX
 *
 */

#pragma once

#include "color.hxx"
#include "name.hxx"
#include "score.hxx"

namespace spadesx {

/**
 * @brief Team data
 *
 */
class team_data : public score_data, public name_data<10>, public color_data
{
};

} // namespace spadesx
