// Weapon_pda.cpp
//

#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION( rvmWeaponObject, rvmWeaponPDA )
END_CLASS

/*
================
rvmWeaponPDA::Init
================
*/
void rvmWeaponPDA::Init( idWeapon* weapon )
{
	rvmWeaponObject::Init( weapon );
}

/*
================
rvmWeaponPDA::Raise
================
*/
stateResult_t rvmWeaponPDA::Raise( stateParms_t* parms )
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
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
================
rvmWeaponPDA::Lower
================
*/
stateResult_t rvmWeaponPDA::Lower( stateParms_t* parms )
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
rvmWeaponPDA::Idle
================
*/
stateResult_t rvmWeaponPDA::Idle( stateParms_t* parms )
{
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch( parms->stage )
	{
		case IDLE_NOTSET:
			owner->GetOwner()->Event_OpenPDA();
			parms->stage = IDLE_WAIT;
			return SRESULT_WAIT;

		case IDLE_WAIT:
			if( !owner->GetOwner()->objectiveSystemOpen )
			{
				//owner->WeaponState( WP_LOWERING, 0 );
				owner->LowerWeapon();
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
}

/*
================
rvmWeaponPDA::Fire
================
*/
stateResult_t rvmWeaponPDA::Fire( stateParms_t* parms )
{
	return SRESULT_DONE;
}

/*
================
rvmWeaponPDA::Reload
================
*/
stateResult_t rvmWeaponPDA::Reload( stateParms_t* parms )
{
	return SRESULT_DONE;
}
