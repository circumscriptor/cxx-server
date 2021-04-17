/**
 *
 * SpadesX
 *
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
template<std::size_t N>
class name_data
{
  public:
    /**
     * @brief Construct a new name data object
     *
     */
    constexpr name_data() noexcept = default;

    /**
     * @brief Set name
     *
     * @param name New name
     * @param max_size Max size
     */
    constexpr void set_name(std::string_view name) noexcept
    {
        const auto* _it  = name.begin();
        const auto* _end = name.end();
        for (m_length = 0; _it != _end && *_it != 0 && m_length < N; ++m_length, ++_it) {
            m_name[m_length] = *_it;
        }
    }

    /**
     * @brief Get name
     *
     * @return Name
     */
    [[nodiscard]] constexpr std::string_view name() const noexcept
    {
        return {m_name.data(), m_length};
    }

  protected:
    std::array<char, N> m_name;      //!< Name
    std::size_t         m_length{0}; //!< Length of the name
};

} // namespace spadesx
