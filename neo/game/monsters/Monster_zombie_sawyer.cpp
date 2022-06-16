// Monster_zombie_sawyer.cpp
//

#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

#define SAWYER_SMOKE_PARTICLES	0
#define SAWYER_BLOOD_PARTICLES	1

CLASS_DECLARATION( rvmMonsterZombie, rvmMonsterZombieSawyer )
END_CLASS

/*
====================
rvmMonsterZombieSawyer::AI_Begin
====================
*/
void rvmMonsterZombieSawyer::AI_Begin( void )
{
	Event_SetSmokeVisibility( ALL_PARTICLES, 0 );
	Event_SetSmokeVisibility( SAWYER_SMOKE_PARTICLES, 1 ); // This was called as a seperate event before; possibly a optimization?
	SetState( "state_Begin" );
}

/*
=====================
rvmMonsterZombieSawyer::state_Begin
=====================
*/
stateResult_t rvmMonsterZombieSawyer::state_Begin( stateParms_t* parms )
{
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 0 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 0 );

	Event_SetMoveType( MOVETYPE_ANIM );
	SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieSawyer::state_Idle
=====================
*/
stateResult_t rvmMonsterZombieSawyer::state_Idle( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		if( wait_for_enemy( parms ) == SRESULT_DONE )
		{
			parms->stage = 1;
		}

		return SRESULT_WAIT;
	}

	Event_SetState( "state_Combat" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieSawyer::check_attacks
=====================
*/
int rvmMonsterZombieSawyer::check_attacks()
{
	int attack_flags;

	attack_flags = 0;
	if( TestMelee() )
	{
		attack_flags |= ATTACK_MELEE;
	}

	return attack_flags;
}

/*
=====================
rvmMonsterZombieSawyer::do_attack
=====================
*/
void rvmMonsterZombieSawyer::do_attack( int attack_flags )
{
	if( attack_flags & ATTACK_MELEE )
	{
		AI_ATTACKING = true;
		SetState( "combat_melee" );
	}
}

/*
=====================
rvmMonsterZombieSawyer::combat_melee
=====================
*/
stateResult_t rvmMonsterZombieSawyer::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		next_hit_time = 0;
		smoke_frames = 0;

		Event_LookAtEnemy( 100 );
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack", 8 );
		//SetWaitState("melee_attack");
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( InAnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack" ) )
		{
			if( gameLocal.SysScriptTime() > next_hit_time )
			{
				next_hit_time = gameLocal.SysScriptTime() + 0.025;
				Event_SetSmokeVisibility( SAWYER_BLOOD_PARTICLES, 1 );
				smoke_frames = 4;
			}

			if( smoke_frames > 0 )
			{
				smoke_frames--;
				if( !smoke_frames )
				{
					Event_SetSmokeVisibility( SAWYER_BLOOD_PARTICLES, 0 );
				}
			}
		}
		else
		{
			parms->stage = 2;
		}
		return SRESULT_WAIT;
	}

	//waitAction("melee_attack");
	Event_SetSmokeVisibility( SAWYER_BLOOD_PARTICLES, 0 );
	Event_LookAtEnemy( 1 );
	//AI_ATTACKING = false;
	return SRESULT_DONE;
}