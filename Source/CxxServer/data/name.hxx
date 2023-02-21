/**
 * @file name.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include <array>
#include <string_view>

namespace spadesx {

/**
 * @brief Name data
 *
 * @tparam N Size
 */
template <std::size_t N>
class name_data
{
  public:

    /**
     * @brief Construct a new name data object
     *
     */
    constexpr name_data() noexcept
        : m_name { 0 }
    {
    }

    /**
     * @brief Set name
     *
     * @param name New name
     */
    constexpr void set_name(std::string_view name) noexcept
    {
        const auto * _it  = name.begin();
        const auto * _end = name.end();
        for (m_name_length = 0; _it != _end && *_it != 0 && m_name_length < N; ++m_name_length, ++_it)
        {
            m_name[m_name_length] = *_it;
        }
    }

    /**
     * @brief Get name
     *
     * @return Name
     */
    [[nodiscard]] constexpr std::string_view name() const noexcept
    {
        return { m_name, m_name_length };
    }

  protected:

    char        m_name[N];           //!< Name
    std::size_t m_name_length { 0 }; //!< Length of the name
};

} // namespace spadesx
