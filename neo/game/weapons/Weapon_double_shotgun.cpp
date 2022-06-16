// Weapon_double_shotgun.cpp
//

#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION( rvmWeaponObject, rvmWeaponDoubleShotgun )
END_CLASS


//#define SHOTGUN_DOUBLE_FIRERATE		1.333
#define SHOTGUN_DOUBLE_FIRERATE		2.2

#define SHOTGUN_DOUBLE_REQUIRED			2

// blend times
#define SHOTGUN_DOUBLE_IDLE_TO_IDLE	0
#define SHOTGUN_DOUBLE_IDLE_TO_LOWER	4
#define SHOTGUN_DOUBLE_IDLE_TO_FIRE	0
#define	SHOTGUN_DOUBLE_IDLE_TO_RELOAD	4
#define	SHOTGUN_DOUBLE_IDLE_TO_NOAMMO	4
#define SHOTGUN_DOUBLE_NOAMMO_TO_RELOAD	4
#define SHOTGUN_DOUBLE_NOAMMO_TO_IDLE	4
#define SHOTGUN_DOUBLE_RAISE_TO_IDLE	4
#define SHOTGUN_DOUBLE_FIRE_TO_IDLE	4
#define SHOTGUN_DOUBLE_RELOAD_TO_IDLE	4
#define	SHOTGUN_DOUBLE_RELOAD_TO_FIRE	4


//Shotgun Projectile Information
#define SHOTGUN_CENTER_PROJECTILES		8
//#define SHOTGUN_CENTER_PROJECTILES 7
#define SHOTGUN_BIG_PROJECTILES			12
//#define SHOTGUN_BIG_PROJECTILES 13

#define SHOTGUN_CENTER_WIDTH			5
#define SHOTGUN_CENTER_HEIGHT			10
//#define SHOTGUN_CENTER_HEIGHT 12
#define SHOTGUN_BIG_WIDTH				22
//#define SHOTGUN_BIG_WIDTH 25
#define SHOTGUN_BIG_HEIGHT				15

/*
===============
rvmWeaponDoubleShotgun::Init
===============
*/
void rvmWeaponDoubleShotgun::Init( idWeapon* weapon )
{
	rvmWeaponObject::Init( weapon );

	next_attack = 0;
}

/*
===============
rvmWeaponDoubleShotgun::Raise
===============
*/
stateResult_t rvmWeaponDoubleShotgun::Raise( stateParms_t* parms )
{
	enum RisingState
	{
		RISING_NOTSET = 0,
		RISING_WAIT
	};

	switch( parms->stage )
	{
		case RISING_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "raise", false );
			parms->stage = RISING_WAIT;
			return SRESULT_WAIT;

		case RISING_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, SHOTGUN_DOUBLE_RAISE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}


/*
===============
rvmWeaponDoubleShotgun::Lower
===============
*/
stateResult_t rvmWeaponDoubleShotgun::Lower( stateParms_t* parms )
{
	enum LoweringState
	{
		LOWERING_NOTSET = 0,
		LOWERING_WAIT
	};

	switch( parms->stage )
	{
		case LOWERING_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "putaway", false );
			parms->stage = LOWERING_WAIT;
			return SRESULT_WAIT;

		case LOWERING_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				SetState( "Holstered" );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
===============
rvmWeaponDoubleShotgun::Idle
===============
*/
stateResult_t rvmWeaponDoubleShotgun::Idle( stateParms_t* parms )
{
	//float currentTime = 0;
	float clip_size;

	clip_size = owner->ClipSize();

	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch( parms->stage )
	{
		case IDLE_NOTSET:
			owner->Event_WeaponReady();
			if( !owner->AmmoInClip() )
			{
				owner->Event_WeaponOutOfAmmo();
			}
			else
			{
				owner->Event_WeaponReady();
			}

			owner->Event_PlayCycle( ANIMCHANNEL_ALL, "idle" );
			parms->stage = IDLE_WAIT;
			return SRESULT_WAIT;

		case IDLE_WAIT:
			// Do nothing.
			return SRESULT_DONE;
	}

	return SRESULT_ERROR;
}

/*
===============
rvmWeaponDoubleShotgun::Fire
===============
*/
stateResult_t rvmWeaponDoubleShotgun::Fire( stateParms_t* parms )
{
	int ammoClip = owner->AmmoInClip();

	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_WAIT
	};

	if( ammoClip == 0 && owner->AmmoAvailable() && parms->stage == 0 )
	{
		//owner->WeaponState( WP_RELOAD, PISTOL_IDLE_TO_RELOAD );
		owner->Reload();
		return SRESULT_DONE;
	}

	switch( parms->stage )
	{
		case FIRE_NOTSET:
			//if (ammoClip == SHOTGUN_LOWAMMO) {
			//	int length;
			//	owner->StartSoundShader(snd_lowammo, SND_CHANNEL_ITEM, 0, false, &length);
			//}

			//owner->Event_LaunchProjectiles(SHOTGUN_NUMPROJECTILES, spread, 0, 1, 1);
			owner->Event_LaunchProjectilesEllipse( SHOTGUN_CENTER_PROJECTILES, SHOTGUN_CENTER_WIDTH, SHOTGUN_CENTER_HEIGHT, 0, 1.0 );
			owner->Event_LaunchProjectilesEllipse( SHOTGUN_BIG_PROJECTILES, SHOTGUN_BIG_WIDTH, SHOTGUN_BIG_HEIGHT, 0, 1.0 );

			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "fire", false );
			parms->stage = FIRE_WAIT;
			return SRESULT_WAIT;

		case FIRE_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, SHOTGUN_DOUBLE_FIRE_TO_IDLE ) )
			{
				owner->Event_WeaponReloading();
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
===============
rvmWeaponDoubleShotgun::Reload
===============
*/
stateResult_t rvmWeaponDoubleShotgun::Reload( stateParms_t* parms )
{
	enum RELOAD_State
	{
		RELOAD_NOTSET = 0,
		RELOAD_WAIT
	};

	switch( parms->stage )
	{
		case RELOAD_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "reload_start", false );
			parms->stage = RELOAD_WAIT;
			return SRESULT_WAIT;

		case RELOAD_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				owner->Event_AddToClip( owner->ClipSize() );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
===============
rvmWeaponDoubleShotgun::EjectBrass
===============
*/
void rvmWeaponDoubleShotgun::EjectBrass( void )
{

}