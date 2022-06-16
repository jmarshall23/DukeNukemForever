// Weapon_flashlight.cpp
//

#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION( rvmWeaponObject, rvmWeaponFlashlight )
END_CLASS

// blend times
#define FLASHLIGHT_IDLE_TO_LOWER	4
#define FLASHLIGHT_IDLE_TO_FIRE		2
#define	FLASHLIGHT_IDLE_TO_RELOAD	4
#define FLASHLIGHT_RAISE_TO_IDLE	4
#define FLASHLIGHT_FIRE_TO_IDLE		4
#define FLASHLIGHT_RELOAD_TO_IDLE	4

#define	FLASHLIGHT_MIN_SKIN_INTENSITY	0.2

/*
================
rvmWeaponFlashlight::Init
================
*/
void rvmWeaponFlashlight::Init( idWeapon* weapon )
{
	rvmWeaponObject::Init( weapon );

	skin_on = owner->GetKey( "skin_on" );
	skin_on_invis = owner->GetKey( "skin_on_invis" );
	skin_off = owner->GetKey( "skin_off" );
	skin_off_invis = owner->GetKey( "skin_off_invis" );

	intensity = 1.0;

	owner->Event_SetLightParm( 3, 1.0 );
	owner->Event_SetShaderParm( 3, 1.0 );

	on = true;

	UpdateSkin();
}

/*
================
rvmWeaponFlashlight::UpdateLightIntensity
================
*/
void rvmWeaponFlashlight::UpdateLightIntensity( void )
{
	// TODO this has to interact with scripts somehow
}

/*
================
rvmWeaponFlashlight::UpdateSkin
================
*/
void rvmWeaponFlashlight::UpdateSkin( void )
{
	if( on && ( intensity > FLASHLIGHT_MIN_SKIN_INTENSITY ) )
	{
		if( !owner->Event_IsInvisible() )
		{
			owner->Event_SetSkin( skin_on );
		}
		else
		{
			owner->Event_SetSkin( skin_on_invis );
		}
	}
	else
	{
		if( !owner->Event_IsInvisible() )
		{
			owner->Event_SetSkin( skin_off );
		}
		else
		{
			owner->Event_SetSkin( skin_off_invis );
		}
	}
}

/*
================
rvmWeaponFlashlight::Raise
================
*/
stateResult_t rvmWeaponFlashlight::Raise( stateParms_t* parms )
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
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, FLASHLIGHT_RAISE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
================
rvmWeaponFlashlight::Lower
================
*/
stateResult_t rvmWeaponFlashlight::Lower( stateParms_t* parms )
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
================
rvmWeaponFlashlight::Idle
================
*/
stateResult_t rvmWeaponFlashlight::Idle( stateParms_t* parms )
{
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch( parms->stage )
	{
		case IDLE_NOTSET:
			owner->Event_WeaponReady();
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
================
rvmWeaponFlashlight::Fire
================
*/
stateResult_t rvmWeaponFlashlight::Fire( stateParms_t* parms )
{
	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_MELEE,
		FIRE_WAIT
	};

	switch( parms->stage )
	{
		case FIRE_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "fire", false );
			parms->stage = FIRE_MELEE;
			parms->Wait( 0.1f );
			return SRESULT_WAIT;

		case FIRE_MELEE:
			owner->Event_Melee();
			parms->stage = FIRE_WAIT;
			return SRESULT_WAIT;

		case FIRE_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, FLASHLIGHT_FIRE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
================
rvmWeaponFlashlight::Reload
================
*/
stateResult_t rvmWeaponFlashlight::Reload( stateParms_t* parms )
{
	enum RELOAD_State
	{
		RELOAD_NOTSET = 0,
		RELOAD_TOGGLEFLASHLIGHT,
		RELOAD_WAIT
	};

	switch( parms->stage )
	{
		case RELOAD_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "reload", false );
			parms->stage = RELOAD_TOGGLEFLASHLIGHT;
			parms->Wait( 0.2f );
			return SRESULT_WAIT;

		case RELOAD_TOGGLEFLASHLIGHT:
			on = !on;
			UpdateSkin();
			owner->Event_Flashlight( on );
			parms->stage = RELOAD_WAIT;
			return SRESULT_WAIT;

		case RELOAD_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, FLASHLIGHT_RELOAD_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}
