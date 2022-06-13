// Weapon_BFG.cpp
//

#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION( rvmWeaponObject, rvmWeaponBFG )
END_CLASS

#define BFG_MINRELEASETIME		0.05
#define BFG_FUSE				2.0
#define BFG_SHORTFUSE			0.5
#define BFG_MAXPOWER			4.0
#define BFG_FIRERATE			4
#define BFG_FIREDELAY			4
#define BFG_NUMPROJECTILES		1

// blend times
#define BFG_IDLE_TO_LOWER		4
#define BFG_IDLE_TO_FIRE		4
#define	BFG_IDLE_TO_RELOAD		4
#define BFG_RAISE_TO_IDLE		4
#define BFG_FIRE_TO_IDLE		4
#define BFG_RELOAD_TO_IDLE		4

/*
===============
rvmWeaponBFG::Init
===============
*/
void rvmWeaponBFG::Init( idWeapon* weapon )
{
	rvmWeaponObject::Init( weapon );

	next_attack = 0;
	fuse_start = 0;
	fuse_end = 0;
	powerLevel = 0;
	fire_time = 0;
	spread = weapon->GetFloat( "spread" );
	owner->Event_SetGuiFloat( "powerlevel", 0 );
	owner->Event_SetGuiFloat( "overcharge", 0 );
}

/*
===============
rvmWeaponBFG::Raise
===============
*/
stateResult_t rvmWeaponBFG::Raise( stateParms_t* parms )
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
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, BFG_RAISE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}


/*
===============
rvmWeaponBFG::Lower
===============
*/
stateResult_t rvmWeaponBFG::Lower( stateParms_t* parms )
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
rvmWeaponBFG::Idle
===============
*/
stateResult_t rvmWeaponBFG::Idle( stateParms_t* parms )
{
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch( parms->stage )
	{
		case IDLE_NOTSET:

			if( !owner->AmmoInClip() )
			{
				owner->Event_PlayCycle( ANIMCHANNEL_ALL, "idle_empty" );
				owner->Event_WeaponOutOfAmmo();
			}
			else
			{
				owner->Event_PlayCycle( ANIMCHANNEL_ALL, "idle" );
				owner->Event_WeaponReady();
			}
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
rvmWeaponBFG::Fire
===============
*/
stateResult_t rvmWeaponBFG::Fire( stateParms_t* parms )
{
	float time_held;
	float power = 0.0f;
	float intensity;
	int ammoClip = owner->AmmoInClip();

	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_FUSING,
		FIRE_SHORTFUSE,
		FIRE_SHORTFUSEWAIT,
		FIRE_FIRE,
		FIRE_FIRE_ANIMWAIT,
		FIRE_FIRE_COOLDOWN,
		FIRE_DONE
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
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "fire_begin", false );
			fuse_start = gameLocal.time;
			fuse_end = gameLocal.time + SEC2MS( BFG_FUSE );
			parms->stage = FIRE_FUSING;
			return SRESULT_WAIT;

		case FIRE_FUSING:
			if( !owner->IsFiring() )
			{
				parms->stage = FIRE_FIRE;
			}
			else if( gameLocal.time > gameLocal.time + SEC2MS( BFG_MINRELEASETIME ) && !owner->IsFiring() )
			{
				parms->stage = FIRE_SHORTFUSE;
			}
			else if( gameLocal.time >= fuse_end )
			{
				parms->stage = FIRE_SHORTFUSE;
			}
			else
			{
				powerLevel = ( gameLocal.time - fuse_start ) / BFG_FUSE;
				owner->Event_SetColor( powerLevel, powerLevel, powerLevel );
				owner->Event_SetGuiFloat( "powerlevel", powerLevel );
			}
			return SRESULT_WAIT;

		case FIRE_SHORTFUSE:
			if( owner->IsFiring() )
			{
				fuse_end = gameLocal.time + SEC2MS( BFG_SHORTFUSE );
				parms->stage = FIRE_SHORTFUSEWAIT;
			}
			else
			{
				parms->stage = FIRE_FIRE;
			}
			return SRESULT_WAIT;

		case FIRE_SHORTFUSEWAIT:
			if( !owner->IsFiring() || gameLocal.time >= fuse_end )
			{
				powerLevel = ( gameLocal.time - fuse_start ) / BFG_FUSE;
				owner->Event_SetGuiFloat( "powerlevel", powerLevel );
				parms->stage = FIRE_FIRE;
			}
			return SRESULT_WAIT;

		case FIRE_FIRE:
			if( gameLocal.time >= fuse_end )
			{
				OverCharge();
				parms->stage = FIRE_DONE;
			}
			else
			{
				time_held = gameLocal.time - fuse_start;
				if( power > BFG_MAXPOWER )
				{
					power = BFG_MAXPOWER;
				}

				owner->Event_PlayAnim( ANIMCHANNEL_ALL, "fire", false );
				fire_time = gameLocal.time;

				owner->Event_LaunchProjectiles( BFG_NUMPROJECTILES, spread, 0, 1, 1 );
				parms->stage = FIRE_FIRE_ANIMWAIT;
			}
			return SRESULT_WAIT;

		case FIRE_FIRE_ANIMWAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, BFG_FIRE_TO_IDLE ) )
			{
				parms->stage = FIRE_FIRE_COOLDOWN;
			}
			else
			{
				intensity = 1 - ( ( gameLocal.time - fire_time ) / 0.5 );
				if( intensity < 0 )
				{
					intensity = 0;
				}
				owner->Event_SetColor( intensity, intensity, intensity );
			}
			return SRESULT_WAIT;

		case FIRE_FIRE_COOLDOWN:
			if( powerLevel <= 0 )
			{
				parms->stage = FIRE_DONE;
			}
			else
			{
				powerLevel -= SEC2MS( 0.05 ); // TODO: DELTA TIME THIS!!
				owner->Event_SetGuiFloat( "powerlevel", powerLevel );
			}
			return SRESULT_WAIT;

		case FIRE_DONE:
			owner->Event_SetGuiFloat( "powerlevel", 0 );
			return SRESULT_DONE;
	}

	return SRESULT_ERROR;
}

/*
===============
rvmWeaponBFG::OverCharge
===============
*/

void rvmWeaponBFG::OverCharge()
{
	idStr	 entname;
	idEntity* explosion;
	idVec3	forward;
	idAngles  angles;
	idPlayer* player;

	player = owner->GetOwner();

	owner->Event_AllowDrop( false );
	owner->Event_UseAmmo( owner->ClipSize() );

	angles = player->viewAngles;
	forward = angles.ToForward();

	entname = owner->GetKey( "def_overcharge" );
	//explosion = sys.spawn(entname);
	{
		idDict	spawnArgs;

		spawnArgs.Set( "classname", entname );
		gameLocal.SpawnEntityDef( spawnArgs, &explosion );
	}
	explosion->Event_SetOrigin( owner->GetOrigin() + forward * 16 );
	explosion->Event_SetShaderParm( /*SHADERPARM_TIMEOFFSET*/4, -gameLocal.time );
	gameLocal.DelayRemoveEntity( explosion, 2 );

	owner->Event_StartSound( "snd_explode", SND_CHANNEL_ANY, false );
	gameLocal.RadiusDamage( owner->GetOrigin(), owner, owner->GetOwner(), nullptr, nullptr, "damage_bfg_overcharge", 1.0 );
}

/*
===============
rvmWeaponBFG::Reload
===============
*/
stateResult_t rvmWeaponBFG::Reload( stateParms_t* parms )
{
	enum RELOAD_State
	{
		RELOAD_NOTSET = 0,
		RELOAD_WAIT
	};

	switch( parms->stage )
	{
		case RELOAD_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "reload", false );
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