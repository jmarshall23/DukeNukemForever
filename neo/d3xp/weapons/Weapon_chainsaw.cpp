// Weapon_chainsaw.cpp
//

#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION( rvmWeaponObject, rvmWeaponChainsaw )
END_CLASS

#define CHAINSAW_FIRERATE			0.1

// blend times
#define CHAINSAW_IDLE_TO_LOWER		4
#define CHAINSAW_IDLE_TO_FIRE		4
#define CHAINSAW_RAISE_TO_IDLE		4
#define CHAINSAW_FIRE_TO_IDLE		4

/*
===============
rvmWeaponChainsaw::Init
===============
*/
void rvmWeaponChainsaw::Init( idWeapon* weapon )
{
	rvmWeaponObject::Init( weapon );

	next_attack = 0;
}


/*
===============
rvmWeaponChainsaw::Raise
===============
*/
stateResult_t rvmWeaponChainsaw::Raise( stateParms_t* parms )
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
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, CHAINSAW_RAISE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}


/*
===============
rvmWeaponChainsaw::Lower
===============
*/
stateResult_t rvmWeaponChainsaw::Lower( stateParms_t* parms )
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
rvmWeaponChainsaw::Idle
===============
*/
stateResult_t rvmWeaponChainsaw::Idle( stateParms_t* parms )
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
===============
rvmWeaponChainsaw::Reload
===============
*/
stateResult_t rvmWeaponChainsaw::Reload( stateParms_t* parms )
{
	return SRESULT_DONE;
}

/*
===============
rvmWeaponChainsaw::Fire
===============
*/
stateResult_t rvmWeaponChainsaw::Fire( stateParms_t* parms )
{
	float currentTime;

	if( parms->stage == 0 )
	{
		owner->Event_PlayAnim( ANIMCHANNEL_ALL, "melee_start", false );
		owner->Event_Melee();
		owner->Event_StartSound( "snd_startattack", SND_CHANNEL_WEAPON, false );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 3 ) )
		{
			parms->stage = 2;
			return SRESULT_WAIT;
		}

		return SRESULT_WAIT;
	}

	if( parms->stage == 2 )
	{
		owner->Event_StartSound( "snd_attack", SND_CHANNEL_WEAPON, false );
		parms->stage = 3;
		return SRESULT_WAIT;
	}

	if( parms->stage == 3 )
	{
		if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
		{
			parms->stage = 4;
			return SRESULT_WAIT;
		}

		return SRESULT_WAIT;
	}

	if( parms->stage == 4 )
	{
		owner->Event_PlayCycle( ANIMCHANNEL_ALL, "melee_loop" );
		next_attack = gameLocal.SysScriptTime();

		parms->stage = 5;

		return SRESULT_WAIT;
	}

	if( parms->stage == 5 )
	{
		if( owner->IsFiring() )
		{
			currentTime = gameLocal.SysScriptTime();
			if( currentTime >= next_attack )
			{
				owner->Event_Melee();
				next_attack = currentTime + CHAINSAW_FIRERATE;
			}
			return SRESULT_WAIT;
		}
		else
		{
			parms->stage = 6;
			return SRESULT_WAIT;
		}
	}

	if( parms->stage == 6 )
	{
		owner->Event_StartSound( "snd_stopattack", SND_CHANNEL_WEAPON, false );
		owner->Event_PlayAnim( ANIMCHANNEL_ALL, "melee_end", false );
		parms->stage = 7;
		return SRESULT_WAIT;
	}

	if( !owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
	{
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}