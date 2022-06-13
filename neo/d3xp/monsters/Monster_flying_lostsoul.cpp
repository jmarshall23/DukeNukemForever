// Monster_flying_lostsoul.cpp
//

#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

#define LOSTSOUL_CHARGE_SPEED		440
#define LOSTSOUL_NORMAL_SPEED		140
#define LOSTSOUL_ATTACK_RATE		2
#define LOSTSOUL_CHARGE_RANGE_MAX	768
#define LOSTSOUL_CHARGE_RANGE_MIN	64
#define LOSTSOUL_NOFOVTIME			4

#define ATTACK_LOSTSOUL_CHARGE	ATTACK_SPECIAL1
#define ATTACK_LOSTSOUL_RETREAT	ATTACK_SPECIAL2

CLASS_DECLARATION( idAI, rvmMonsterLostSoul )
END_CLASS

/*
====================
rvmMonsterLostSoul::Init
====================
*/
void rvmMonsterLostSoul::Init( void )
{

}

/*
=================
rvmMonsterLostSoul::AI_Begin
=================
*/
void rvmMonsterLostSoul::AI_Begin( void )
{
	fly_offset = GetFloatKey( "fly_offset" );
	Event_SetState( "state_Begin" );
}

/*
=====================
rvmMonsterLostSoul::state_Begin
=====================
*/
stateResult_t rvmMonsterLostSoul::state_Begin( stateParms_t* parms )
{
	Event_SetMoveType( MOVETYPE_FLY );
	noMeleeTime = 0;
	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 0 );
	Event_SetState( "state_Idle" );
	return SRESULT_DONE;
}

/*
=====================
rvmMonsterLostSoul::state_Idle
=====================
*/
stateResult_t rvmMonsterLostSoul::state_Idle( stateParms_t* parms )
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
rvmMonsterLostSoul::do_attack
=====================
*/
void rvmMonsterLostSoul::do_attack( int attack_flags )
{
	nextNoFOVAttack = gameLocal.SysScriptTime() + LOSTSOUL_NOFOVTIME;
	if( attack_flags & ATTACK_LOSTSOUL_RETREAT )
	{
		stateThread.SetState( "combat_retreat" );
	}
	else if( attack_flags & ATTACK_MELEE )
	{
		stateThread.SetState( "combat_melee" );
	}
	else if( attack_flags & ATTACK_LOSTSOUL_CHARGE )
	{
		stateThread.SetState( "combat_charge" );
	}
}

/*
=====================
rvmMonsterLostSoul::check_attacks
=====================
*/
int rvmMonsterLostSoul::check_attacks()
{
	float range;
	float currentTime;
	int attack_flags;

	attack_flags = 0;
	currentTime = gameLocal.SysScriptTime();

	if( currentTime >= noMeleeTime )
	{
		if( TestMelee() )
		{
			attack_flags |= ATTACK_MELEE;
		}
	}

	if( ( ( gameLocal.SysScriptTime() > nextNoFOVAttack ) && AI_ENEMY_VISIBLE ) || AI_ENEMY_IN_FOV )
	{
		range = EnemyRange2D();
		if( range < 40 )
		{
			attack_flags |= ATTACK_LOSTSOUL_RETREAT;
		}

		if( ( range < LOSTSOUL_CHARGE_RANGE_MAX && range > 200 ) && ( !CanReachEnemy() || ( currentTime >= nextAttack ) ) )
		{
			if( TestChargeAttack() )
			{
				attack_flags |= ATTACK_LOSTSOUL_CHARGE;
			}
		}
	}

	return attack_flags;
}

/*
=====================
rvmMonsterLostSoul::combat_charge
=====================
*/
stateResult_t rvmMonsterLostSoul::combat_charge( stateParms_t* parms )
{
	// rise up a bit
	if( parms->stage == 0 )
	{
		Event_SetFlyOffset( fly_offset + 200 );
		parms->Wait( 0.5f );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( parms->stage == 1 )
	{
		Event_FaceEnemy();

		// determin direction to enemy
		idVec3 pos = GetEnemyEyePos();
		idVec3 org = GetOrigin();
		idVec3 forward = pos - org;
		forward.z = 0;
		forward.Normalize();

		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Charge", 4 );

		// set speed and height
		vel = forward * LOSTSOUL_CHARGE_SPEED;
		Event_SetFlyOffset( fly_offset );
		Event_SetFlySpeed( LOSTSOUL_CHARGE_SPEED );

		parms->stage = 2;
		return SRESULT_WAIT;
	}

	if( parms->stage == 2 )
	{
		if( InAnimState( ANIMCHANNEL_TORSO, "Torso_Charge" ) )
		{
			// do a dive bomb run
			idVec3 org = GetOrigin();
// jmarshall - need to fix this, causing "huge translation" issue.
			//vel.z = -(org.z - pos.z + 16) * 3;
// jmarshall end
			Event_SetLinearVelocity( vel );
			return SRESULT_WAIT;
		}
	}

	// restore the fly speed
	Event_SetFlySpeed( LOSTSOUL_NORMAL_SPEED );

	// don't attack for a bit
	nextAttack = gameLocal.DelayTime( LOSTSOUL_ATTACK_RATE );
	nextNoFOVAttack = gameLocal.SysScriptTime() + LOSTSOUL_NOFOVTIME;

	// don't allow melee for a bit so that charge attacks don't cause double damage
	noMeleeTime = gameLocal.SysScriptTime() + 1;

	return SRESULT_DONE;
}
/*
=====================
rvmMonsterLostSoul::combat_melee
=====================
*/
stateResult_t rvmMonsterLostSoul::combat_melee( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_FaceEnemy();
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_MeleeAttack", 4 );
		SetWaitState( "melee_attack" );
		parms->stage = 1;
		return SRESULT_WAIT;
	}

	if( waitState != "" )
	{
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
=====================
rvmMonsterLostSoul::combat_retreat
=====================
*/
stateResult_t rvmMonsterLostSoul::combat_retreat( stateParms_t* parms )
{
	if( parms->stage == 0 )
	{
		Event_MoveOutOfRange( GetEnemy(), EnemyRange2D() + 256 );
		endtime = gameLocal.SysScriptTime() + 1;
		parms->stage = 1;
		return SRESULT_WAIT;
	}


	if( endtime < gameLocal.SysScriptTime() )
	{
		if( AI_MOVE_DONE )
		{
			return SRESULT_DONE;
		}
	}

	return SRESULT_DONE;
}

/*
================================================

Lost Soul Animation Code

================================================
*/

/*
===================
rvmMonsterLostSoul::Torso_Idle
===================
*/
stateResult_t rvmMonsterLostSoul::Torso_Idle(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_PlayCycle(ANIMCHANNEL_TORSO, "idle");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AI_PAIN)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 0);
			return SRESULT_DONE;
		}
		if (AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Fly", 4);
			return SRESULT_DONE;
		}

		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterLostSoul::Torso_Fly
===================
*/
stateResult_t rvmMonsterLostSoul::Torso_Fly(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_PlayCycle(ANIMCHANNEL_TORSO, "fly");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AI_PAIN)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 0);
			return SRESULT_DONE;
		}
		if (!AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 4);
			return SRESULT_DONE;
		}

		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
rvmMonsterLostSoul::Torso_Charge
===================
*/
stateResult_t rvmMonsterLostSoul::Torso_Charge(stateParms_t* parms) {
	float dist;
	float endtime;

	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
		STAGE_END
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_PlayCycle(ANIMCHANNEL_TORSO, "charge");
		Event_FadeSound(SND_CHANNEL_VOICE2, 0, 0);
		Event_StartSound("snd_charge", SND_CHANNEL_VOICE2, false);
		dist = DistanceTo(enemy.GetEntity()->GetOrigin()) + 256;
		endtime = gameLocal.SysScriptTime() + dist / LOSTSOUL_CHARGE_SPEED;

		parms->param1 = endtime;

		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		endtime = parms->param1;
		if (!AI_PAIN && (gameLocal.SysScriptTime() < endtime))
		{
			if (gameLocal.InfluenceActive())
			{
				parms->stage = STAGE_END;
				return SRESULT_WAIT;
			}		

			if (TestMelee())
			{
				AttackMelee("melee_lostsoul_charge");
				parms->stage = STAGE_END;
				return SRESULT_WAIT;
			}

			return SRESULT_WAIT;
		}

		parms->stage = STAGE_END;
		return SRESULT_WAIT;

	case STAGE_END:
		Event_EndAttack();
		Event_FadeSound(SND_CHANNEL_VOICE2, -60, 1);
		Event_FinishAction("charge_attack");

		Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 4);
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}