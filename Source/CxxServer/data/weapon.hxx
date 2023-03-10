/**
 * @file weapon.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "enums.hxx"

#include <cstdint>

namespace spadesx {

/**
 * @brief Weapon parameters
 *
 */
class weapon_params
{
  public:

    /**
     * @brief Construct a new weapon data object
     *
     * @param head Damage on head hit
     * @param torso Damage on torso hit
     * @param arms Damage on arms hit
     * @param legs Damage on legs hit
     * @param clip Clip ammo
     * @param stock Stock ammo
     */
    constexpr weapon_params(std::uint8_t head, std::uint8_t torso, std::uint8_t arms, std::uint8_t legs, std::uint8_t clip, std::uint8_t stock) noexcept
        : m_damage_head { head }
        , m_damage_torso { torso }
        , m_damage_arms { arms }
        , m_damage_legs(legs)
        , m_ammo_clip { clip }
        , m_ammo_stock { stock }
    {
    }

    /**
     * @brief Get weapon damage
     *
     * @param hit Hit type
     * @return Weapon damage
     */
    [[nodiscard]] constexpr std::uint8_t get_damage(hit_type hit) const noexcept
    {
        switch (hit)
        {
            case hit_type::head:
                return m_damage_head;
            case hit_type::torso:
                return m_damage_torso;
            case hit_type::arms:
                return m_damage_arms;
            case hit_type::legs:
                return m_damage_legs;
            default:
                return 0;
        }
    }

    /**
     * @brief Get clip ammo amount
     *
     * @return Clip ammo
     */
    [[nodiscard]] constexpr std::uint8_t get_clip() const noexcept
    {
        return m_ammo_clip;
    }

    /**
     * @brief Get stock ammo amount
     *
     * @return Stock ammo
     */
    [[nodiscard]] constexpr std::uint8_t get_stock() const noexcept
    {
        return m_ammo_stock;
    }

  private:

    const std::uint8_t m_damage_head;
    const std::uint8_t m_damage_torso;
    const std::uint8_t m_damage_arms;
    const std::uint8_t m_damage_legs;
    const std::uint8_t m_ammo_clip;
    const std::uint8_t m_ammo_stock;
};

/**
 * @brief Weapon data collection
 *
 */
class weapons
{
  public:

    /**
     * @brief Get weapon data
     *
     * @param weapon Weapon
     * @return Weapon data
     */
    static constexpr const weapon_params & get(weapon_type weapon) noexcept
    {
        switch (weapon)
        {
            case weapon_type::rifle:
                return m_rifle;
            case weapon_type::smg:
                return m_smg;
            case weapon_type::shotgun:
                return m_shotgun;
        }
    }

  private:

    static constexpr const weapon_params m_rifle   = { 100, 49, 33, 33, 10, 50 };
    static constexpr const weapon_params m_smg     = { 75, 29, 18, 18, 30, 120 };
    static constexpr const weapon_params m_shotgun = { 27, 37, 16, 16, 6, 48 };
};

/**
 * @brief Get clip ammo amount
 *
 * @param weapon Weapon
 * @return Clip ammo
 */
static constexpr std::uint8_t get_weapon_clip(weapon_type weapon)
{
    return weapons::get(weapon).get_clip();
}

/**
 * @brief Get stock ammo amount
 *
 * @param weapon Weapon
 * @return Stock ammo
 */
static constexpr std::uint8_t get_weapon_stock(weapon_type weapon)
{
    return weapons::get(weapon).get_stock();
}

/**
 * @brief Get weapon damage
 *
 * @param weapon Weapon damage
 * @param hit Hit type
 * @return Weapon damage
 */
static constexpr std::uint8_t get_weapon_damage(weapon_type weapon, hit_type hit)
{
    return weapons::get(weapon).get_damage(hit);
}

} // namespace spadesx
