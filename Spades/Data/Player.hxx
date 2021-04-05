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
    Vector3f mPosition;        //!< Position
    Vector3f mOrientation;     //!< Orientation
    Weapon   mWeapon;          //!< Weapon
    TeamType mTeam;            //!< Team
    Tool     mTool{Tool::Gun}; //!< Currently held tool
    Color3b  mColor;           //!< Block color
    uint32   mKills{0};        //!< Player kills
    uint8    mRespawnTime{0};  //!< Current respawn time

    struct LastKill
    {
        uint8    mKiller;
        KillType mType;
    } mLastKill;

    struct Input
    {
        bool mUp{false};
        bool mDown{false};
        bool mLeft{false};
        bool mRight{false};
        bool mJump{false};
        bool mCrouch{false};
        bool mSneak{false};
        bool mSprint{false};
        bool mPrimary{false};   //!< Primary action
        bool mSecondary{false}; //!< Secondary action
    } mInput;

    uint8 mClipAmmo{0};      //!< Clip ammo
    uint8 mReserveAmmo{0};   //!< Reserve ammo
    bool  mCrouching{false}; //!< Crouching state
    bool  mGliding{false};   //!< In the air (airborne)

    bool mAlive{false};    //!< Is player alive
    bool mCanSpawn{false}; //!< Is player allowed to spawn
    // bool mWaitingForSpawn{false}; //!< Is player waiting to be respawned

    State mState{State::Disconnected}; //!< Player state

    Player(uint8 id) : mID{id}
    {
    }

    void Reset()
    {
        mPosition         = {0.f, 0.f, 0.f};
        mOrientation      = {0.f, 0.f, 0.f};
        mState            = State::Disconnected;
        mTool             = Tool::Gun;
        mInput.mUp        = false;
        mInput.mDown      = false;
        mInput.mLeft      = false;
        mInput.mRight     = false;
        mInput.mJump      = false;
        mInput.mCrouch    = false;
        mInput.mSneak     = false;
        mInput.mSprint    = false;
        mInput.mPrimary   = false;
        mInput.mSecondary = false;
        mCrouching        = false;
        mGliding          = false;
        // mWaitingForSpawn  = false;
        mCanSpawn = false;
        mAlive    = false;
    }
};

} // namespace Spades
