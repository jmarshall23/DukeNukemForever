// Monster_flying_cacodemon.cpp
//

#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

#define CACO_ATTACK_RATE	3
#define CACO_NOFOVTIME		4

CLASS_DECLARATION( idAI, rvmMonsterFlyingCacodemon )
END_CLASS

/*
=================
rvmMonsterFlyingCacodemon::Init
=================
*/
void rvmMonsterFlyingCacodemon::Init( void )
{
}

/*
=================
rvmMonsterFlyingCacodemon::AI_Begin
=================
*/
void rvmMonsterFlyingCacodemon::AI_Begin( void )
{
	Event_SetState( "state_Idle" );
}

/*
=====================
rvmMonsterFlyingCacodemon::state_Begin
=====================
*/
stateResult_t rvmMonsterFlyingCacodemon::state_Begin( stateParms_t* parms )
{
	Event_SetMoveType( MOVETYPE_FLY );
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 8 );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterFlyingCacodemon::state_Idle
=====================
*/
stateResult_t rvmMonsterFlyingCacodemon::state_Idle( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		if( wait_for_enemy( parms ) == SRESULT_DONE )
		{
			parms->stage = 1;
		}

		return SRESULT_WAIT;
	}

	nextAttack = 0;
	nextNoFOVAttack = 0;

	Event_SetState( "state_Combat" );
	return SRESULT_DONE;
}


/*
=====================
rvmMonsterFlyingCacodemon::do_attack
=====================
*/
void rvmMonsterFlyingCacodemon::do_attack( int attack_flags )
{
	nextNoFOVAttack = gameLocal.SysScriptTime() + CACO_NOFOVTIME;
	if( attack_flags & ATTACK_COMBAT_NODE )
	{
		//combat_ainode(combat_node);
		gameLocal.Error( "Combat_Node fix me\n" );
	}
	else if( attack_flags & ATTACK_MELEE )
	{
		SetState( "combat_melee" );
	}
	else if( attack_flags & ATTACK_MISSILE )
	{
		SetState( "combat_range" );
	}
}

/*
=====================
rvmMonsterFlyingCacodemon::check_attacks
=====================
*/
int rvmMonsterFlyingCacodemon::check_attacks()
{
	float currentTime;
	float canMelee;
	int attack_flags;

	attack_flags = 0;

	canMelee = TestMelee();
	currentTime = gameLocal.SysScriptTime();
	if( !canMelee )
	{
		combat_node = GetCombatNode();
		if( combat_node )
		{
			attack_flags |= ATTACK_COMBAT_NODE;
		}
	}

	if( canMelee )
	{
		attack_flags |= ATTACK_MELEE;
	}

	if( ( ( gameLocal.SysScriptTime() > nextNoFOVAttack ) && AI_ENEMY_VISIBLE ) || AI_ENEMY_IN_FOV )
	{
		if( !CanReachEnemy() || ( currentTime >= nextAttack ) )
		{
			if( CanHitEnemyFromAnim( "range_attack" ) )
			{
				attack_flags |= ATTACK_MISSILE;
			}
		}
	}

	return attack_flags;
}

/*
=====================
monster_zombie::combat_range
=====================
*/
stateResult_t rvmMonsterFlyingCacodemon::combat_range( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4 );
		SetWaitState( "range_attack" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( waitState == "" )
		{
			parms->stage = 2;
		}
		return SRESULT_WAIT;
	}

	// don't attack for a bit
	nextAttack = gameLocal.DelayTime( CACO_ATTACK_RATE );
	nextNoFOVAttack = gameLocal.SysScriptTime() + CACO_NOFOVTIME;

	return SRESULT_DONE;
}

/*
=====================
monster_zombie::combat_melee
=====================
*/
stateResult_t rvmMonsterFlyingCacodemon::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack", 4 );
		SetWaitState( "melee_attack" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( waitState == "" )
		{
			parms->stage = 2;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

