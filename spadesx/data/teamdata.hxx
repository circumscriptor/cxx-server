/**
 *
 * SpadesX
 *
 */

#pragma once

#include "colordata.hxx"
#include "namedata.hxx"
#include "scoredata.hxx"

namespace spadesx {

/**
 * @brief Team data
 *
 */
class team_data : public score_data, public name_data<10>, public color_data
{
};

} // namespace spadesx
