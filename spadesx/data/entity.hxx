/**
 * @file entity.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "enums.hxx"
#include "position.hxx"

namespace spadesx {

/**
 * @brief Object IDs
 *
 */
class object_id
{
  public:
    static constexpr const std::uint8_t team_a_intel = 0;    //!< ID of team a's intel
    static constexpr const std::uint8_t team_b_intel = 1;    //!< ID of team b's intel
    static constexpr const std::uint8_t team_a_base  = 2;    //!< ID of team a's base
    static constexpr const std::uint8_t team_b_base  = 3;    //!< ID of team b's base
    static constexpr const std::uint8_t invalid      = 0xFF; //!< Invalid ID

    /**
     * @brief Get intel ID (CTF mode)
     *
     * @param team Team
     * @return Intel ID
     */
    static constexpr std::uint8_t intel_id(team_type team) noexcept
    {
        switch (team) {
            case team_type::a:
                return team_a_intel;
            case team_type::b:
                return team_b_intel;
            default:
                return invalid;
        }
    }

    /**
     * @brief Get base ID (CTF mode)
     *
     * @param team Team
     * @return Base ID
     */
    static constexpr std::uint8_t base_id(team_type team) noexcept
    {
        switch (team) {
            case team_type::a:
                return team_a_base;
            case team_type::b:
                return team_b_base;
            default:
                return invalid;
        }
    }
};

/**
 * @brief Entity (intel, base, player)
 *
 */
class entity : public position_data
{
  public:
    /**
     * @brief Construct a new entity object
     *
     * @param type Entity type
     * @param id ID
     * @param team Team
     */
    constexpr entity(entity_type type, std::uint8_t id, team_type team) noexcept : m_type{type}, m_id{id}, m_team{team}
    {
    }

    /**
     * @brief Get entity type
     *
     * @return Entity type
     */
    [[nodiscard]] constexpr entity_type type() const noexcept
    {
        return m_type;
    }

    /**
     * @brief Get ID
     *
     * @return ID
     */
    [[nodiscard]] constexpr std::uint8_t id() const noexcept
    {
        return m_id;
    }

    /**
     * @brief Get team
     *
     * @return Team
     */
    [[nodiscard]] constexpr team_type team() const noexcept
    {
        return m_team;
    }

    /**
     * @brief Change team
     *
     * @param team Team
     */
    constexpr void change_team(team_type team) noexcept
    {
        m_team = team;
    }

    /**
     * @brief Change team to another entity's team
     *
     * @param other Other entity
     */
    void change_to_same(const entity& other) noexcept
    {
        m_team = other.m_team;
    }

    /**
     * @brief Check whether this entity is in the same team as another entity
     *
     * @param other Other entity
     * @return true If is in the same team
     */
    [[nodiscard]] constexpr bool is_same_team(const entity& other) const noexcept
    {
        return m_team == other.m_team;
    }

  protected:
    entity_type  m_type; //!< Entity type
    std::uint8_t m_id;   //!< Entity's id
    team_type    m_team; //!< Entity's team
};

} // namespace spadesx
