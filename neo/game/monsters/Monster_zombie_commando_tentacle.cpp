// Monster_zombie_command_tentcle.cpp
//

#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

#define ZCT_RUNDISTANCE			128
#define ZCT_WALKTURN			65
#define ZCT_ATTACK_RATE			5
#define ZCT_TENTACLE_RANGE_MAX	200
#define ZCT_NOFOVTIME			4

#define ATTACK_TENTACLE			ATTACK_SPECIAL1

CLASS_DECLARATION( rvmMonsterZombie, rvmMonsterZombieCommandoTentacle )
END_CLASS

/*
=================
rvmMonsterZombieCommandoTentacle::Init
=================
*/
void rvmMonsterZombieCommandoTentacle::Init( void )
{

}

/*
=================
rvmMonsterZombieCommandoTentacle::AI_Begin
=================
*/
void rvmMonsterZombieCommandoTentacle::AI_Begin( void )
{
	run_distance = ZCT_RUNDISTANCE;
	walk_turn = ZCT_WALKTURN;

	can_run = true;

	Event_SetState( "state_Begin" );
}

/*
=====================
rvmMonsterZombieCommandoTentacle::state_Begin
=====================
*/
stateResult_t rvmMonsterZombieCommandoTentacle::state_Begin( stateParms_t* parms )
{
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 8 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 8 );

	Event_SetMoveType( MOVETYPE_ANIM );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieCommandoTentacle::state_Idle
=====================
*/
stateResult_t rvmMonsterZombieCommandoTentacle::state_Idle( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		if( wait_for_enemy( parms ) == SRESULT_DONE )
		{
			parms->stage = 1;
		}

		return SRESULT_WAIT;
	}

	tentacleDamage = false;
	nextAttack = 0;
	nextNoFOVAttack = 0;

	Event_SetState( "state_Combat" );
	return SRESULT_DONE;
}

/*
=====================
monster_zombie::combat_melee
=====================
*/
stateResult_t rvmMonsterZombieCommandoTentacle::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		tentacleDamage = false;
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
			Event_LookAtEnemy( 1 );
			if( tentacleDamage )
			{
				if( MeleeAttackToJoint( "joint13", "melee_commandoTentacle" ) )
				{
					tentacleDamage = false;
				}
			}
		}
		else
		{
			parms->stage = 2;
		}
		return SRESULT_WAIT;
	}

	tentacleDamage = false;

	AI_ATTACKING = false;
	return SRESULT_DONE;
}


/*
=====================
monster_zombie::combat_tentacle
=====================
*/
stateResult_t rvmMonsterZombieCommandoTentacle::combat_tentacle( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		tentacleDamage = false;
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_TentacleAttack", 4 );
		//SetWaitState("melee_attack");
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( InAnimState( ANIMCHANNEL_TORSO, "Torso_TentacleAttack" ) )
		{
			if( tentacleDamage )
			{
				if( MeleeAttackToJoint( "joint13", "melee_commandoTentacle" ) )
				{
					tentacleDamage = false;
				}
			}
		}
		else
		{
			parms->stage = 2;
		}
		return SRESULT_WAIT;
	}

	tentacleDamage = false;

	AI_ATTACKING = false;
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieCommandoTentacle::check_attacks
=====================
*/
int rvmMonsterZombieCommandoTentacle::check_attacks()
{
	int attack_flags;
	float range;
	float currentTime;

	attack_flags = 0;
	if( TestMelee() )
	{
		attack_flags |= ATTACK_MELEE;
	}

	if( ( ( gameLocal.SysScriptTime() > nextNoFOVAttack ) && AI_ENEMY_VISIBLE ) || AI_ENEMY_IN_FOV )
	{
		range = EnemyRange();
		currentTime = gameLocal.SysScriptTime();
		if( ( range < ZCT_TENTACLE_RANGE_MAX ) && ( !CanReachEnemy() || ( currentTime >= nextAttack ) ) )
		{
			if( CanHitEnemy() )
			{
				attack_flags |= ATTACK_TENTACLE;
			}
		}
	}

	return attack_flags;
}

/*
=====================
rvmMonsterZombieCommandoTentacle::do_attack
=====================
*/
void rvmMonsterZombieCommandoTentacle::do_attack( int attack_flags )
{
	if( attack_flags & ATTACK_MELEE )
	{
		AI_ATTACKING = true;
		SetState( "combat_melee" );
	}
	else if( attack_flags & ATTACK_TENTACLE )
	{
		SetState( "combat_tentacle" );
	}
}

/*
=====================
rvmMonsterZombieCommandoTentacle::tentacle_attack_start

Called from md5Anim frame via TypeInfoGen invoke
=====================
*/
void rvmMonsterZombieCommandoTentacle::tentacle_attack_start()
{
	tentacleDamage = true;
}

/*
=====================
rvmMonsterZombieCommandoTentacle::tentacle_attack_end

Called from md5Anim frame via TypeInfoGen invoke
=====================
*/
void rvmMonsterZombieCommandoTentacle::tentacle_attack_end()
{
	tentacleDamage = false;
}

/*
================================================

Animation States

================================================
*/

/*
===================
rvmMonsterZombieCommandoTentacle::Torso_TentacleAttack
===================
*/
stateResult_t rvmMonsterZombieCommandoTentacle::Torso_TentacleAttack(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_DisablePain();
		Event_FaceEnemy();
		Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 4))
		{
			Event_FinishAction("tentacle_attack");
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}