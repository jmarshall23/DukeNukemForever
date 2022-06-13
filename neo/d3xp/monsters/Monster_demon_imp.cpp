// monster_demon_imp.cpp
//


#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

#define IMP_RUNDISTANCE		192
#define IMP_WALKTURN		65
#define IMP_ATTACK_RATE		1
#define IMP_LEAP_RATE		7
#define IMP_LEAP_SPEED		650
#define IMP_LEAP_MAXHEIGHT	48
#define IMP_DODGE_RATE		8
#define IMP_LEAP_RANGE_MIN	200
#define IMP_LEAP_RANGE_MAX	480
#define IMP_NOFOVTIME		4

CLASS_DECLARATION( idAI, rvmMonsterDemonImp )
END_CLASS

/*
=================
rvmMonsterDemonImp::Init
=================
*/
void rvmMonsterDemonImp::Init( void )
{
	//jumpVelocity.LinkTo( scriptObject, "jumpVelocity" );
	//range_attack_anim.LinkTo( scriptObject, "range_attack_anim" );
	range_attack_anim = "";
	jumpVelocity.Zero();
	//canSwitchToIdleFromRange = true;
}

/*
=================
rvmMonsterDemonImp::AI_Begin
=================
*/
void rvmMonsterDemonImp::AI_Begin( void )
{
	run_distance = IMP_RUNDISTANCE;
	walk_turn = IMP_WALKTURN;

	Event_SetState( "state_Begin" );
}

/*
=====================
rvmMonsterDemonImp::state_Begin
=====================
*/
stateResult_t rvmMonsterDemonImp::state_Begin( stateParms_t* parms )
{
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 0 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 0 );

	Event_SetMoveType( MOVETYPE_ANIM );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterDemonImp::state_Idle
=====================
*/
stateResult_t rvmMonsterDemonImp::state_Idle( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		if( wait_for_enemy( parms ) == SRESULT_DONE )
		{
			parms->stage = 1;
		}

		return SRESULT_WAIT;
	}

	nextLeap = gameLocal.RandomTime( IMP_LEAP_RATE );
	nextAttack = 0;
	nextDodge = gameLocal.RandomTime( IMP_DODGE_RATE );
	nextNoFOVAttack = 0;


	Event_SetState( "state_Combat" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterDemonImp::do_attack
=====================
*/
void rvmMonsterDemonImp::do_attack( int attack_flags )
{
	nextNoFOVAttack = gameLocal.SysScriptTime() + IMP_NOFOVTIME;
	if( attack_flags & ATTACK_DODGE_LEFT )
	{
		SetState( "combat_dodge_left" );
	}
	else if( attack_flags & ATTACK_DODGE_RIGHT )
	{
		SetState( "combat_dodge_right" );
	}
	else if( attack_flags & ATTACK_COMBAT_NODE )
	{
		//combat_ainode(combat_node);
		gameLocal.Error( "combat_ainode" );
	}
	else if( attack_flags & ATTACK_MELEE )
	{
		SetState( "combat_melee" );
	}
	else if( attack_flags & ATTACK_LEAP )
	{
		SetState( "combat_leap" );
	}
	else if( attack_flags & ATTACK_MISSILE )
	{
		SetState( "combat_range" );
	}
}

/*
=====================
rvmMonsterDemonImp::check_attacks
=====================
*/
int rvmMonsterDemonImp::check_attacks()
{
	float range;
	float currentTime;
	float canMelee;
	int attack_flags;
	float checkLeap;
	idVec3 vel;
	float t;
	idStr anim;
	idVec3 jumpTarget;

	attack_flags = 0;

	canMelee = TestMelee();
	currentTime = gameLocal.SysScriptTime();
	if( !canMelee )
	{
		if( AI_PAIN && ( currentTime >= nextDodge ) )
		{
			if( TestAnimMove( "evade_left" ) )
			{
				attack_flags |= ATTACK_DODGE_LEFT;
			}
			if( TestAnimMove( "evade_right" ) )
			{
				attack_flags |= ATTACK_DODGE_RIGHT;

				// if we can dodge either direction, pick one
				if( attack_flags & ATTACK_DODGE_LEFT )
				{
					if( gameLocal.Random( 100 ) < 50 )
					{
						attack_flags &= ~ATTACK_DODGE_RIGHT;
					}
					else
					{
						attack_flags &= ~ATTACK_DODGE_LEFT;
					}
				}
			}
		}

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
		range = EnemyRange();
		if( ( range >= IMP_LEAP_RANGE_MIN ) && ( range < IMP_LEAP_RANGE_MAX ) && ( currentTime >= nextLeap ) )
		{
			if( CanHitEnemy() )
			{
				t = AnimLength( ANIMCHANNEL_TORSO, "jump_start" );
				jumpTarget = PredictEnemyPos( t );
				idVec3 _jumpVelocity = GetJumpVelocity( jumpTarget, IMP_LEAP_SPEED, IMP_LEAP_MAXHEIGHT );
				if( _jumpVelocity != vec3_origin )
				{
					attack_flags |= ATTACK_LEAP;
				}
				else
				{
					// check if we can leap again in 2 seconds
					nextLeap = gameLocal.DelayTime( 2 );
				}

				jumpVelocity = _jumpVelocity;
			}
		}

		if( !CanReachEnemy() || ( currentTime >= nextAttack ) )
		{
// jmarshall: can't pass idStr to doomscript for some reason, currently hardcoded in script.
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
rvmMonsterDemonImp::combat_range
=====================
*/
stateResult_t rvmMonsterDemonImp::combat_range( stateParms_t* parms )
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
		if( waitState != "" )
		{
			return SRESULT_WAIT;
		}
	}

	// don't attack for a bit
	nextAttack = gameLocal.DelayTime( IMP_ATTACK_RATE );
	nextNoFOVAttack = gameLocal.SysScriptTime() + IMP_NOFOVTIME;
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterDemonImp::combat_leap
=====================
*/
stateResult_t rvmMonsterDemonImp::combat_leap( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_StopMove();
		Event_TurnToPos( GetOrigin() + jumpVelocity );
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_LeapAttack", 4 );
		SetWaitState( "leap_attack" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( waitState != "" )
		{
			return SRESULT_WAIT;
		}
	}

	nextLeap = gameLocal.DelayTime( IMP_LEAP_RATE );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterDemonImp::combat_melee
=====================
*/
stateResult_t rvmMonsterDemonImp::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_LookAtEnemy( 100 );
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack", 5 );
		SetWaitState( "melee_attack" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( waitState != "" )
		{
			return SRESULT_WAIT;
		}
	}

	Event_LookAtEnemy( 1 );
	Event_StopMove();
	return SRESULT_DONE;
}

/*
=====================
monster_demon_imp::combat_dodge_left
=====================
*/
stateResult_t rvmMonsterDemonImp::combat_dodge_left( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		Event_StopMove();
		Event_AnimState( ANIMCHANNEL_LEGS, "Legs_DodgeLeft", 2 );
		SetWaitState( "strafe" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( waitState != "" )
		{
			return SRESULT_WAIT;
		}
	}

	nextDodge = gameLocal.DelayTime( IMP_DODGE_RATE );
}

/*
=====================
monster_demon_imp::combat_dodge_right
=====================
*/
stateResult_t rvmMonsterDemonImp::combat_dodge_right( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		Event_StopMove();
		Event_AnimState( ANIMCHANNEL_LEGS, "Legs_DodgeRight", 2 );
		SetWaitState( "strafe" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( waitState != "" )
		{
			return SRESULT_WAIT;
		}
	}

	nextDodge = gameLocal.DelayTime( IMP_DODGE_RATE );
}

/*
================================================

Animation States

================================================
*/

/*
===================
rvmMonsterDemonImp::Torso_LeapAttack
===================
*/
stateResult_t rvmMonsterDemonImp::Torso_LeapAttack(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
		STAGE_WAITGROUND,
		STAGE_FINISH
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_OverrideAnim(ANIMCHANNEL_LEGS);
		Event_DisablePain();
		Event_PlayAnim(ANIMCHANNEL_TORSO, "jump_start");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 2))
		{
			Event_SetBlendFrames(ANIMCHANNEL_TORSO, 2);
			Event_BeginAttack("melee_impLeapAttack");
			Event_PlayCycle(ANIMCHANNEL_TORSO, "jump_loop");
			parms->stage = STAGE_WAITGROUND;
		}
		return SRESULT_WAIT;

	case STAGE_WAITGROUND:
		if (AI_ONGROUND)
		{
			Event_EndAttack();
			Event_PlayAnim(ANIMCHANNEL_TORSO, "jump_end");
			parms->stage = STAGE_FINISH;
		}
		return SRESULT_WAIT;

	case STAGE_FINISH:
		if (AnimDone(ANIMCHANNEL_TORSO, 4))
		{
			Event_FinishAction("leap_attack");
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterDemonImp::Legs_Idle
===================
*/
stateResult_t rvmMonsterDemonImp::Legs_Idle(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_IdleAnim(ANIMCHANNEL_LEGS, "stand");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (can_run && AI_RUN && AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Run", 8);
			return SRESULT_DONE;
		}
		if (AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Walk", 8);
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterDemonImp::Legs_DodgeLeft
===================
*/
stateResult_t rvmMonsterDemonImp::Legs_DodgeLeft(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_IdleAnim(ANIMCHANNEL_LEGS, "evade_left");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_LEGS, 4))
		{
			Event_FinishAction("strafe");
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Idle", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterDemonImp::Legs_DodgeRight
===================
*/
stateResult_t rvmMonsterDemonImp::Legs_DodgeRight(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_IdleAnim(ANIMCHANNEL_LEGS, "evade_right");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_LEGS, 4))
		{
			Event_FinishAction("strafe");
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Idle", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}