// Monster_zombie_commando_cgun.cpp
//

#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

#define ZOMBIE_CGUN_RUNDISTANCE					192
#define ZOMBIE_CGUN_WALKTURN					65
#define ZOMBIE_CGUN_DODGE_RATE					3
#define ZOMBIE_CGUN_ATTACK_DELAY_MIN			2.5
#define ZOMBIE_CGUN_ATTACK_DELAY_MAX			4
#define ZOMBIE_CGUN_ATTACK_MAX_LENGTH			2.5
#define ZOMBIE_CGUN_ATTACK_MIN_LENGTH			1
#define ZOMBIE_CGUN_WAIT_MIN_LENGTH				0.3
#define ZOMBIE_CGUN_WAIT_MAX_LENGTH				1.5
#define ZOMBIE_CGUN_CROUCH_ATTACK_MAX_LENGTH	15
#define ZOMBIE_CGUN_CROUCH_ATTACK_MIN_LENGTH	3
#define ZOMBIE_CGUN_STAND_ATTACK_MAX_LENGTH		15
#define ZOMBIE_CGUN_STAND_ATTACK_MIN_LENGTH		3
#define ZOMBIE_CGUN_MIN_SHOT_TIME				0.5
#define ZOMBIE_CGUN_NOFOVTIME					4

#define ATTACK_ZCC_CROUCHFIRE					ATTACK_SPECIAL1

CLASS_DECLARATION( idAI, rvmMonsterZombieCommandoChaingun )
END_CLASS

/*
========================
rvmMonsterZombieCommandoChaingun::Init
========================
*/
void rvmMonsterZombieCommandoChaingun::Init( void )
{
	fire = false;
	crouch_fire = false;
	step_left = false;
	step_right = false;
	nextDodge = false;
	nextAttack = false;
	nextNoFOVAttack = false;
	combat_node = NULL;
}

/*
========================
rvmMonsterZombieCommandoChaingun::AI_Begin
========================
*/
void rvmMonsterZombieCommandoChaingun::AI_Begin( void )
{
	run_distance = ZOMBIE_CGUN_RUNDISTANCE;
	walk_turn = ZOMBIE_CGUN_WALKTURN;

	Event_SetState( "state_Begin" );
}

/*
=====================
rvmMonsterZombieCommandoChaingun::state_Begin
=====================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::state_Begin( stateParms_t* parms )
{
	fire = false;
	crouch_fire = false;
	step_left = false;

	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 8 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 8 );

	Event_SetMoveType( MOVETYPE_ANIM );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieCommandoChaingun::state_Idle
=====================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::state_Idle( stateParms_t* parms )
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
	nextDodge = gameLocal.RandomTime( ZOMBIE_CGUN_DODGE_RATE );

	Event_SetState( "state_Combat" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieCommandoChaingun::do_attack
=====================
*/
void rvmMonsterZombieCommandoChaingun::do_attack( int attack_flags )
{
	nextNoFOVAttack = gameLocal.SysScriptTime() + ZOMBIE_CGUN_NOFOVTIME;
	if( attack_flags & ATTACK_DODGE_LEFT )
	{
		stateThread.SetState( "combat_dodge_left" );
	}
	else if( attack_flags & ATTACK_DODGE_RIGHT )
	{
		//combat_dodge_right();
		stateThread.SetState( "combat_dodge_right" );
	}
	else if( attack_flags & ATTACK_COMBAT_NODE )
	{
		//combat_ainode(combat_node);
		gameLocal.Error( "rvmMonsterZombieSecurityPistol::CombatAINode\n" );
	}
	else if( attack_flags & ATTACK_MISSILE )
	{
		//stand_attack();
		stateThread.SetState( "stand_attack" );
	}
	else if( attack_flags & ATTACK_ZCC_CROUCHFIRE )
	{
		//crouch_attack();
		//stateThread.SetState( "crouch_attack" );
		stateThread.SetState("stand_attack");
	}
}


/*
=====================
rvmMonsterZombieCommandoChaingun::check_attacks
=====================
*/
int rvmMonsterZombieCommandoChaingun::check_attacks()
{
	float currentTime;
	int attack_flags;
	boolean try_attack;
	boolean reachable;

	attack_flags = 0;

	currentTime = gameLocal.SysScriptTime();
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

	reachable = CanReachEnemy();
	if( AI_ENEMY_IN_FOV && ( !reachable || ( currentTime >= nextAttack ) ) )
	{
		try_attack = true;
	}
	else if( ( gameLocal.SysScriptTime() > nextNoFOVAttack ) && AI_ENEMY_VISIBLE && ( !reachable || ( currentTime >= nextAttack ) ) )
	{
		try_attack = true;
	}
	else if( Touches( GetEnemy() ) )
	{
		try_attack = true;
	}
	else
	{
		try_attack = false;
	}

	if( try_attack )
	{
		if( gameLocal.Random( 1 ) < 0.5 )
		{
			if( CanHitEnemyFromAnim( "crouch_range_attack_loop" ) )
			{
				attack_flags |= ATTACK_ZCC_CROUCHFIRE;
			}
			else if( CanHitEnemyFromAnim( "range_attack_loop" ) )
			{
				attack_flags |= ATTACK_MISSILE;
			}
		}
		else if( CanHitEnemyFromAnim( "range_attack_loop" ) )
		{
			attack_flags |= ATTACK_MISSILE;
		}
	}

	return attack_flags;
}


/*
=====================
rvmMonsterZombieCommandoChaingun::stand_attack
=====================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::stand_attack( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		fire = true;
		Event_FaceEnemy();
		Event_AnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 8);
		attackTime = gameLocal.RandomDelay( ZOMBIE_CGUN_STAND_ATTACK_MIN_LENGTH, ZOMBIE_CGUN_STAND_ATTACK_MAX_LENGTH );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( AI_ENEMY_VISIBLE )
		{
			parms->stage = 2;
			parms->param1 = gameLocal.RandomDelay( 2, 5 ); // endtime
		}

		parms->stage = 3;
		return SRESULT_WAIT;
	}

	if( parms->stage == 2 )
	{
		float endtime = parms->param1;
		if( gameLocal.SysScriptTime() < endtime )
		{
			if( gameLocal.InfluenceActive() )
			{
				fire = false;
				parms->stage = 1; // break
			}

			if( !CanHitEnemyFromAnim( "range_attack_loop" ) )
			{
				parms->stage = 1; // break
			}
			if( AI_PAIN )
			{
				int attack_flags = check_attacks();
				if( attack_flags & ( ATTACK_DODGE_LEFT | ATTACK_DODGE_RIGHT ) )
				{
					fire = false;
					do_attack( attack_flags );
					return SRESULT_DONE;
				}
			}
		}
		else if( InAnimState( ANIMCHANNEL_TORSO, "Torso_Idle" ) )
		{
			Event_LookAtEnemy( 1.0f );
			return SRESULT_WAIT;
		}
		else
		{
			if( gameLocal.InfluenceActive() )
			{
				parms->stage = 3;
				return SRESULT_WAIT;
			}

			if( gameLocal.SysScriptTime() > attackTime )
			{
				parms->stage = 3;
				return SRESULT_WAIT;
			}

			if( !AI_ENEMY_VISIBLE || !CanHitEnemyFromAnim( "range_attack_loop" ) )
			{
				parms->stage = 3;
				return SRESULT_WAIT;
			}

			step_left = TestAnimMove( "step_left" );
			step_right = TestAnimMove( "step_right" );
			if( step_left && step_right )
			{
				if( gameLocal.Random( 100 ) < 50 )
				{
					step_left = false;
				}
				else
				{
					step_right = false;
				}
			}
			return SRESULT_WAIT;
		}

		return SRESULT_WAIT;
	}

	if( parms->stage == 3 )
	{
		fire = false;
		if( InAnimState( ANIMCHANNEL_TORSO, "Torso_Idle" ) )
		{
			return SRESULT_WAIT;
		}

		goto done;
	}
done:
	// don't attack for a bit
	nextAttack = gameLocal.RandomDelay( ZOMBIE_CGUN_ATTACK_DELAY_MIN, ZOMBIE_CGUN_ATTACK_DELAY_MAX );
	nextNoFOVAttack = gameLocal.SysScriptTime() + ZOMBIE_CGUN_NOFOVTIME;
	return SRESULT_DONE;
}


/*
=====================
rvmMonsterZombieCommandoChaingun::crouch_attack
=====================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::crouch_attack( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		crouch_fire = true;

		attackTime = gameLocal.RandomDelay( ZOMBIE_CGUN_CROUCH_ATTACK_MIN_LENGTH, ZOMBIE_CGUN_CROUCH_ATTACK_MAX_LENGTH );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		if( AI_ENEMY_VISIBLE )
		{
			if( gameLocal.InfluenceActive() )
			{
				crouch_fire = false;
				parms->stage = 2; // break
				return SRESULT_WAIT;
			}
			if( gameLocal.SysScriptTime() > attackTime )
			{
				parms->stage = 2; // break
				return SRESULT_WAIT;
			}

			if( AI_PAIN )
			{
				int attack_flags = check_attacks();
				if( attack_flags & ( ATTACK_DODGE_LEFT | ATTACK_DODGE_RIGHT ) )
				{
					crouch_fire = false;
					Event_SetAnimPrefix( "" );
					do_attack( attack_flags );
					stateThread.PostState( "state_Combat" );
					return SRESULT_DONE;
				}
			}

			Event_LookAtEnemy( 1 );
			if( !CanHitEnemyFromAnim( "range_attack_loop" ) )
			{
				parms->stage = 2; // break
				return SRESULT_WAIT;
			}
		}
		else
		{
			parms->stage = 2;
		}

		return SRESULT_WAIT;
	}

	if( parms->stage == 2 )
	{
		crouch_fire = false;
		if( InAnimState( ANIMCHANNEL_TORSO, "Torso_CrouchAttack" ) )
		{
			return SRESULT_WAIT;
		}
		goto done;
	}
done:
	// don't attack for a bit
	nextAttack = gameLocal.RandomDelay( ZOMBIE_CGUN_ATTACK_DELAY_MIN, ZOMBIE_CGUN_ATTACK_DELAY_MAX );
	nextNoFOVAttack = gameLocal.SysScriptTime() + ZOMBIE_CGUN_NOFOVTIME;

	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieCommandoChaingun::combat_dodge_left
=====================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::combat_dodge_left( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_StopMove();
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_LEGS, "Legs_DodgeLeft", 8 );
		SetWaitState( "strafe" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( waitState != "" )
	{
		return SRESULT_WAIT;
	}

	parms->stage = 2;

	nextDodge = gameLocal.DelayTime( ZOMBIE_CGUN_DODGE_RATE );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterZombieCommandoChaingun::combat_dodge_right
=====================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::combat_dodge_right( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_StopMove();
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_LEGS, "Legs_DodgeRight", 8 );
		SetWaitState( "strafe" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( waitState != "" )
	{
		return SRESULT_WAIT;
	}

	parms->stage = 2;

	nextDodge = gameLocal.DelayTime( ZOMBIE_CGUN_DODGE_RATE );
	return SRESULT_DONE;
}

/*
================================================

Chaingun Zombie Animation Code

================================================
*/

/*
===================
rvmMonsterZombieCommandoChaingun::Torso_Idle
===================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::Torso_Idle(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_IdleAnim(ANIMCHANNEL_TORSO, "range_attack_aim");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AI_PAIN)
		{
			PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
			PostAnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 4);
			return SRESULT_DONE;
		}

		if (fire)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 8);
			return SRESULT_DONE;
		}

		if (crouch_fire)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_CrouchAttack", 8);
			return SRESULT_DONE;
		}

		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterZombieCommandoChaingun::Torso_RangeAttack
===================
*/
stateResult_t rvmMonsterZombieCommandoChaingun::Torso_RangeAttack(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
		STAGE_ATTACKLOOP,
		STAGE_BEGINEND,
		STAGE_WAIT2
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_SetAnimPrefix("");
		Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 12))
		{
			Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack_loop");
			parms->param1 = gameLocal.RandomDelay(ZOMBIE_CGUN_ATTACK_MIN_LENGTH, ZOMBIE_CGUN_ATTACK_MAX_LENGTH);
			parms->stage = STAGE_ATTACKLOOP;
			return SRESULT_DONE;
		}
		else
		{
			if (AI_PAIN)
			{
				PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
				PostAnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4);
				return SRESULT_DONE;
			}
		}

		return SRESULT_WAIT;
	case STAGE_ATTACKLOOP:
		{
			bool isDone = (gameLocal.SysScriptTime() < parms->param1) && CanHitEnemyFromAnim("range_attack_loop");

			if (!isDone)
			{
				if (AnimDone(ANIMCHANNEL_TORSO, 8))
				{
					Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack_loop");
				}
				else
				{
					if (gameLocal.InfluenceActive())
					{
						parms->stage = STAGE_BEGINEND;
						return SRESULT_WAIT;
					}

					if (AI_PAIN)
					{
						PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
						PostAnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4);
						return SRESULT_DONE;
					}
				}
			}
			else
			{
				parms->stage = STAGE_BEGINEND;
			}
		}
		return SRESULT_WAIT;

	case STAGE_BEGINEND:
		if (!AnimDone(ANIMCHANNEL_TORSO, 8)) {
			Event_SetBlendFrames(ANIMCHANNEL_TORSO, 8);
		}
		Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack_end");
		parms->stage = STAGE_WAIT2;
		return SRESULT_WAIT;

	case STAGE_WAIT2:
		if (AnimDone(ANIMCHANNEL_TORSO, 8))
		{
			Event_FinishAction("turret_attack");
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 8);
			return SRESULT_DONE;
		}

		if (AI_PAIN)
		{
			PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
			PostAnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4);
			return SRESULT_DONE;
		}

		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterZombieCommandoChaingun::Torso_CrouchAttack
===================
*/
#if 0
stateResult_t rvmMonsterZombieCommandoChaingun::Torso_CrouchAttack(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
		STAGE_ATTACKLOOP,
		STAGE_BEGINEND,
		STAGE_WAIT2
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_OverrideAnim(ANIMCHANNEL_LEGS);
		Event_SetAnimPrefix("crouch");
		Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 12))
		{
			Event_SetAnimPrefix("");
			Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack_loop");
			parms->param1 = gameLocal.RandomDelay(ZOMBIE_CGUN_ATTACK_MIN_LENGTH, ZOMBIE_CGUN_ATTACK_MAX_LENGTH);
			parms->stage = STAGE_ATTACKLOOP;
			return SRESULT_DONE;
		}
		else
		{
			if (AI_PAIN)
			{
				Event_SetAnimPrefix("");
				PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
				PostAnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4);
				return SRESULT_DONE;
			}
		}

		return SRESULT_WAIT;
	case STAGE_ATTACKLOOP:
	{
		bool isDone = (gameLocal.SysScriptTime() < parms->param1) && crouch_fire;

		if (!isDone)
		{
			if (AnimDone(ANIMCHANNEL_TORSO, 8))
			{
				Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack_loop");
			}
			else
			{
				if (gameLocal.InfluenceActive())
				{
					parms->stage = STAGE_BEGINEND;
					return SRESULT_WAIT;
				}

				if (AI_PAIN)
				{
					Event_SetAnimPrefix("");
					PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
					PostAnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4);
					return SRESULT_DONE;
				}
			}
		}
		else
		{
			parms->stage = STAGE_BEGINEND;
		}
	}
	return SRESULT_WAIT;

	case STAGE_BEGINEND:
		if (!AnimDone(ANIMCHANNEL_TORSO, 8)) {
			Event_SetBlendFrames(ANIMCHANNEL_TORSO, 8);
		}
		Event_PlayAnim(ANIMCHANNEL_TORSO, "range_attack_end");
		parms->stage = STAGE_WAIT2;
		return SRESULT_WAIT;

	case STAGE_WAIT2:
		if (AnimDone(ANIMCHANNEL_TORSO, 8))
		{
			Event_SetAnimPrefix("");
			Event_FinishAction("turret_attack");
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 8);
			return SRESULT_DONE;
		}

		if (AI_PAIN)
		{
			Event_SetAnimPrefix("");
			PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 4);
			PostAnimState(ANIMCHANNEL_TORSO, "Torso_RangeAttack", 4);
			return SRESULT_DONE;
		}

		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}
#endif