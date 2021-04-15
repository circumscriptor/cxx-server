/**
 *
 * SpadesX
 *
 */

#pragma once

#include <string>
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
     * @brief Set name
     *
     * @param name New name
     * @param max_size Max size
     */
    void set_name(std::string_view name)
    {
        auto size = std::min(name.size(), N);
        m_name.reserve(N);
        m_name.assign(name.data(), size);
    }

    /**
     * @brief Get name
     *
     * @return Name
     */
    [[nodiscard]] std::string_view name() const noexcept
    {
        return m_name;
    }

  protected:
    std::string m_name; //!< Name
};

} // namespace spadesx
