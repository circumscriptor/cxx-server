/**
 *
 * SpadesX
 *
 */

#pragma once

#include "enums.hxx"

#include <cstdint>

namespace spadesx {

struct rifle
{
    static constexpr const std::uint8_t damage_head  = 100;
    static constexpr const std::uint8_t damage_torso = 49;
    static constexpr const std::uint8_t damage_arms  = 33;
    static constexpr const std::uint8_t damage_legs  = 33;
    static constexpr const std::uint8_t ammo_clip    = 10;
    static constexpr const std::uint8_t ammo_stock   = 50;
};

struct smg
{
    static constexpr const std::uint8_t damage_head  = 75;
    static constexpr const std::uint8_t damage_torso = 29;
    static constexpr const std::uint8_t damage_arms  = 18;
    static constexpr const std::uint8_t damage_legs  = 18;
    static constexpr const std::uint8_t ammo_clip    = 30;
    static constexpr const std::uint8_t ammo_stock   = 120;
};

struct shotgun
{
    static constexpr const std::uint8_t damage_head  = 27;
    static constexpr const std::uint8_t damage_torso = 37;
    static constexpr const std::uint8_t damage_arms  = 16;
    static constexpr const std::uint8_t damage_legs  = 16;
    static constexpr const std::uint8_t ammo_clip    = 6;
    static constexpr const std::uint8_t ammo_stock   = 48;
};

/**
 * @brief Access weapon properties
 *
 * @tparam T Weapon type
 */
template<typename T>
struct weapon_access
{
    using weapon = T; //!< Weapon

    /**
     * @brief Get clip ammo amount
     *
     * @return Clip ammo
     */
    static constexpr std::uint8_t get_clip()
    {
        return weapon::ammo_clip;
    }

    /**
     * @brief Get stock ammo amount
     *
     * @return Stock ammo
     */
    static constexpr std::uint8_t get_stock()
    {
        return weapon::ammo_stock;
    }

    /**
     * @brief Get weapon damage
     *
     * @param hit Hit type
     * @return Weapon damage
     */
    static constexpr std::uint8_t get_damage(hit_type hit)
    {
        switch (hit) {
            case hit_type::head:
                return weapon::damage_head;
            case hit_type::torso:
                return weapon::damage_torso;
            case hit_type::arms:
                return weapon::damage_arms;
            case hit_type::legs:
                return weapon::damage_legs;
            default:
                return 0;
        }
    }
};

/**
 * @brief Get clip ammo amount
 *
 * @param weapon Weapon
 * @return Clip ammo
 */
static constexpr std::uint8_t get_weapon_clip(weapon_type weapon)
{
    switch (weapon) {
        case weapon_type::rifle:
            return weapon_access<rifle>::get_clip();
        case weapon_type::smg:
            return weapon_access<smg>::get_clip();
        case weapon_type::shotgun:
            return weapon_access<shotgun>::get_clip();
    }
}

/**
 * @brief Get clip ammo amount
 *
 * @param weapon Weapon
 * @return Clip ammo
 */
static constexpr std::uint8_t get_weapon_stock(weapon_type weapon)
{
    switch (weapon) {
        case weapon_type::rifle:
            return weapon_access<rifle>::get_stock();
        case weapon_type::smg:
            return weapon_access<smg>::get_stock();
        case weapon_type::shotgun:
            return weapon_access<shotgun>::get_stock();
    }
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
    switch (weapon) {
        case weapon_type::rifle:
            return weapon_access<rifle>::get_damage(hit);
        case weapon_type::smg:
            return weapon_access<smg>::get_damage(hit);
        case weapon_type::shotgun:
            return weapon_access<shotgun>::get_damage(hit);
    }
}

} // namespace spadesx
