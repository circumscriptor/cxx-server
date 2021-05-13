/**
 * @file command.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

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
    virtual ~command() = default;

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
     * @param connection Source connection
     * @param protocol Protocol
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

/**
 * @brief Register commands
 *
 */
class command_manager
{
  public:
    /**
     * @brief Register command
     *
     * @tparam T Command type
     * @param name Command name
     * @param enable If true, call command->enable()
     */
    template<typename T>
    void register_command(const std::string& name, bool enable = true)
    {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            std::shared_ptr<command> shared = std::make_shared<T>();
            if (enable) {
                shared->enable();
            }
            m_commands.insert({name, shared});
        }
    }

    /**
     * @brief Check whether command is registered
     *
     * @param name Command name
     * @return true, if command is registered
     */
    bool has_command(const std::string& name) const
    {
        auto it = m_commands.find(name);
        return it != m_commands.end();
    }

    /**
     * @brief Get command
     *
     * @param name Command name
     */
    command& get_command(const std::string& name)
    {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            throw std::runtime_error("command not found");
        }
        return *(it->second.get());
    }

    /**
     * @brief Parse command
     *
     * @param command Command
     * @param connection Source connection
     * @param protocol Protocol
     * @return true on success
     */
    bool execute_command(std::string_view command, connection& connection, base_protocol& protocol)
    {
        if (command.empty() || command[0] != '/') {
            return false;
        }

        auto n    = command.find(' ', 1);
        auto name = command.substr(1, n - 1);
        auto size = name.size();
        if (name.back() == 0) {
            size -= 1;
        }

        if (!name.empty()) {
            auto it = m_commands.find(std::string{name.data(), size});
            if (it != m_commands.end()) {
                auto& cmd = *(it->second.get());
                if (cmd.is_enabled()) {
                    auto args = command.substr(n + 1);
                    cmd.on_execute(args, connection, protocol);
                    return true;
                }
            }
        }
        return false;
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<command>> m_commands; //!< Command
};

} // namespace spadesx
