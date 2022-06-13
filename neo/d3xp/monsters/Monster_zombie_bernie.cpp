// Monster_zombie_bernie.cpp
//


#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

CLASS_DECLARATION( rvmMonsterZombie, rvmMonsterZombieBernie )
END_CLASS

/*
=================
rvmMonsterZombieBernie::Init
=================
*/
void rvmMonsterZombieBernie::Init( void )
{

}

/*
=================
rvmMonsterZombieBernie::AI_Begin
=================
*/
void rvmMonsterZombieBernie::AI_Begin( void )
{
	Event_SetState( "state_Begin" );
}

/*
=====================
rvmMonsterZombieBernie::state_Begin
=====================
*/
stateResult_t rvmMonsterZombieBernie::state_Begin( stateParms_t* parms )
{
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 8 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 8 );

	Event_SetMoveType( MOVETYPE_ANIM );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieBernie::state_Idle
=====================
*/
stateResult_t rvmMonsterZombieBernie::state_Idle( stateParms_t* parms )
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
monster_zombie::combat_melee
=====================
*/
stateResult_t rvmMonsterZombieBernie::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_LookAtEnemy( 100 );
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack", 8 );
		//SetWaitState("melee_attack");
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( AnimDone( ANIMCHANNEL_TORSO, 8 ) )
		{
			parms->stage = 2;
		}
		return SRESULT_WAIT;
	}

	//waitAction("melee_attack");
	Event_LookAtEnemy( 1 );
	AI_ATTACKING = false;
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieBernie::check_attacks
=====================
*/
int rvmMonsterZombieBernie::check_attacks()
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
rvmMonsterZombieBernie::do_attack
=====================
*/
void rvmMonsterZombieBernie::do_attack( int attack_flags )
{
	if( attack_flags & ATTACK_MELEE )
	{
		AI_ATTACKING = true;
		SetState( "combat_melee" );
	}
}