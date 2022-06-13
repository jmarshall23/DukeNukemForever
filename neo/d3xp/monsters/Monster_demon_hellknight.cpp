
#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

#define HELLKNIGHT_ATTACK_RATE			3
#define	HELLKNIGHT_PAIN_DELAY			0.25
#define HELLKNIGHT_NOFOVTIME			6

#define HELLKNIGHT_IDLE_TO_RANGEATTACK	4
#define HELLKNIGHT_WALK_TO_MELEE		4
#define HELLKNIGHT_WALK_TO_RANGEATTACK	4

CLASS_DECLARATION( idAI, rvmMonsterDemonHellknight )
END_CLASS

/*
=================
rvmMonsterDemonHellknight::Init
=================
*/
void rvmMonsterDemonHellknight::Init( void )
{
	//range_attack_anim.LinkTo( scriptObject, "range_attack_anim" );
}

/*
=================
rvmMonsterDemonHellknight::AI_Begin
=================
*/
void rvmMonsterDemonHellknight::AI_Begin( void )
{
	Event_SetState( "state_Begin" );
}

/*
=====================
rvmMonsterDemonHellknight::state_Begin
=====================
*/
stateResult_t rvmMonsterDemonHellknight::state_Begin( stateParms_t* parms )
{
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 0 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 0 );

	Event_SetMoveType( MOVETYPE_ANIM );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterDemonHellknight::state_Idle
=====================
*/
stateResult_t rvmMonsterDemonHellknight::state_Idle( stateParms_t* parms )
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
rvmMonsterDemonHellknight::do_attack
=====================
*/
void rvmMonsterDemonHellknight::do_attack( int attack_flags )
{
	nextNoFOVAttack = gameLocal.SysScriptTime() + HELLKNIGHT_NOFOVTIME;
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
rvmMonsterDemonHellknight::check_attacks
=====================
*/
int rvmMonsterDemonHellknight::check_attacks()
{
	float	currentTime;
	float	canMelee;
	int	attack_flags;
	idVec3	vel;
	float	t;
	idVec3	jumpTarget;
	idStr	anim;

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
// jmarshall: I've temp disabled range_attack_anim. I can't seem to pass in idStr to doomscript?
			range_attack_anim = ChooseAnim( ANIMCHANNEL_LEGS, "turret_attack" );
			if( CanHitEnemyFromAnim( range_attack_anim ) )
			{
				attack_flags |= ATTACK_MISSILE;
			}

			anim = ChooseAnim( ANIMCHANNEL_LEGS, "range_attack" );
			if( TestAnimMoveTowardEnemy( anim ) )
			{
				if( CanHitEnemyFromAnim( anim ) )
				{
					range_attack_anim = anim;
					attack_flags |= ATTACK_MISSILE;
				}
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
stateResult_t rvmMonsterDemonHellknight::combat_range( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		if( !AI_ENEMY_IN_FOV )
		{
			parms->Wait( 0.4 );
		}

		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_RangeAttack", HELLKNIGHT_WALK_TO_RANGEATTACK );
		SetWaitState( "range_attack" );
		parms->stage = 2;
		return SRESULT_WAIT;
	}

	if( parms->stage == 2 )
	{
		if( AnimDone( ANIMCHANNEL_TORSO, HELLKNIGHT_WALK_TO_RANGEATTACK ) )
		{
			parms->stage = 3;
		}
		return SRESULT_WAIT;
	}

	// don't attack for a bit
	nextAttack = gameLocal.DelayTime( HELLKNIGHT_ATTACK_RATE );
	nextNoFOVAttack = gameLocal.SysScriptTime() + HELLKNIGHT_NOFOVTIME;

	return SRESULT_DONE;
}

/*
=====================
monster_zombie::combat_melee
=====================
*/
stateResult_t rvmMonsterDemonHellknight::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_LookAtEnemy( 100 );
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack", HELLKNIGHT_WALK_TO_MELEE );
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

	Event_LookAtEnemy( 1 );

	return SRESULT_DONE;
}

