/**
 *
 * SpadesX
 *
 */

#pragma once

#include <any>
#include <memory>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace spadesx {

class connection;
class base_protocol;

/**
 * @brief Command
 *
 */
class command
{
  public:
    /**
     * @brief Destroy the command object
     *
     */
    virtual ~command()
    {
        if (m_enabled) {
            disable();
        }
    }

    /**
     * @brief On command enable
     *
     */
    virtual void on_enable()
    {
    }

    /**
     * @brief On command disable
     *
     */
    virtual void on_disable()
    {
    }

    /**
     * @brief On execute
     *
     * @param args Args
     */
    virtual void on_execute(std::string_view args, connection& connection, base_protocol& protocol)
    {
    }

    /**
     * @brief Return true if is enabled
     *
     * @return true If enabled
     */
    [[nodiscard]] bool is_enabled() const noexcept
    {
        return m_enabled;
    }

    /**
     * @brief Enable
     *
     */
    bool enable()
    {
        if (!m_enabled) {
            on_enable();
            m_enabled = true;
            return true;
        }
        return false;
    }

    /**
     * @brief Disable
     *
     */
    bool disable()
    {
        if (m_enabled) {
            on_disable();
            m_enabled = false;
            return true;
        }
        return false;
    }

  private:
    bool m_enabled; //!< Is enabled
};

} // namespace spadesx
