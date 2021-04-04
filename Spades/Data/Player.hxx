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
    uint32   mID;             //!< Player ID
    char     mName[17];       //!< Player name
    Vector3f mPosition;       //!< Position
    Vector3f mOrientation;    //!< Orientation
    Weapon   mWeapon;         //!< Weapon
    TeamType mTeam;           //!< Team
    Tool     mTool;           //!< Currently held tool
    Color3b  mColor;          //!< Block color
    uint32   mKills{0};       //!< Player kills
    uint8    mRespawnTime{0}; //!< Current respawn time

    struct Input
    {
        bool mUp;
        bool mDown;
        bool mLeft;
        bool mRight;
        bool mJump;
        bool mCrouch;
        bool mSneak;
        bool mSprint;
    } mKeys;

    uint8 mClipAmmo;
    uint8 mReserveAmmo;

    bool mPrimary{false};   //!< Primary action
    bool mSecondary{false}; //!< Secondary action
    bool mCrouching{false}; //!< Crouching state
    bool mGliding{false};   //!< In the air (airborne)

    State mState{State::Disconnected}; //!< Player state
};

} // namespace Spades
