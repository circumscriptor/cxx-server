/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"
#include "Core/Vector.hxx"
#include "Enums.hxx"

namespace Spades {

/**
 * @brief Player data
 *
 */
struct Player
{
    uint8    mID;              //!< Player ID
    char     mName[17];        //!< Player name
    Vector3  mPosition;        //!< Position
    Vector3  mOrientation;     //!< Orientation
    Weapon   mWeapon;          //!< Weapon
    TeamType mTeam;            //!< Team
    Tool     mTool{Tool::Gun}; //!< Currently held tool
    Color3   mColor;           //!< Block color
    uint32   mKills{0};        //!< Player kills
    uint8    mRespawnTime{0};  //!< Current respawn time
    uint8    mClipAmmo{0};     //!< Clip ammo
    uint8    mReserveAmmo{0};  //!< Reserve ammo

    // server-side state
    bool  mCrouching{false};           //!< Crouching state
    bool  mGliding{false};             //!< In the air (airborne)
    bool  mAlive{false};               //!< Is player alive
    bool  mCanSpawn{false};            //!< Is player allowed to spawn
    State mState{State::Disconnected}; //!< Player state

    // Last death
    uint8    mLastKillKiller; //!< Killer, player who killed this player
    KillType mLastKillType;   //!< Type of the kill last kill (death type)

    // Input
    bool mUp{false};        //!< Input forward
    bool mDown{false};      //!< Input backward
    bool mLeft{false};      //!< Input left
    bool mRight{false};     //!< Input right
    bool mJump{false};      //!< Input jump
    bool mCrouch{false};    //!< Input crouch
    bool mSneak{false};     //!< Input sneak
    bool mSprint{false};    //!< Input sprint
    bool mPrimary{false};   //!< Primary action
    bool mSecondary{false}; //!< Secondary action

    /**
     * @brief Construct a new Player object
     *
     * @param id Assigned player ID
     */
    Player(uint8 id) : mID{id}
    {
    }

    /**
     * @brief Reset player to initial state
     *
     */
    void Reset()
    {
        mPosition    = {0.f, 0.f, 0.f};
        mOrientation = {0.f, 0.f, 0.f};
        mState       = State::Disconnected;
        mTool        = Tool::Gun;
        mUp          = false;
        mDown        = false;
        mLeft        = false;
        mRight       = false;
        mJump        = false;
        mCrouch      = false;
        mSneak       = false;
        mSprint      = false;
        mPrimary     = false;
        mSecondary   = false;
        mCrouching   = false;
        mGliding     = false;
        mCanSpawn    = false;
        mAlive       = false;
    }
};

} // namespace Spades
