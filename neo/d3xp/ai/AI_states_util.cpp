// AI_states_util.cpp
//

#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

/*
=====================
idAI::combat_chase
=====================
*/
stateResult_t idAI::combat_chase( stateParms_t* parms, bool& result )
{
	float	delta;
	float	range;
	float	attack_flags;

	if( parms->substage == 0 )
	{
		if( !AI_ENEMY_VISIBLE || ( EnemyRange() > run_distance ) )
		{
			parms->subparam2 = true;
		}
		else
		{
			parms->subparam2 = false;
		}

		Event_MoveToEnemy();
		if( AI_MOVE_DONE )
		{
			result = false;
			return SRESULT_DONE;
		}

		if( AI_MOVE_DONE )
		{
			attack_flags = check_attacks();
			if( attack_flags )
			{
				do_attack( attack_flags );
				result = true;
				return SRESULT_DONE;
			}
			result = false;
			return SRESULT_DONE;
		}

		parms->substage = 1;
		return SRESULT_WAIT;
	}



	if( !AI_MOVE_DONE && !AI_DEST_UNREACHABLE )
	{
		if( AI_ENEMY_DEAD )
		{
			enemy_dead();
		}

		if( gameLocal.InfluenceActive() )
		{
			result = true;
			return SRESULT_DONE;
		}

		if( AI_ENEMY_IN_FOV )
		{
			Event_LookAtEnemy( 1 );
		}

		attack_flags = check_attacks();
		if( attack_flags )
		{
			do_attack( attack_flags );
			stateThread.PostState( "state_Combat" );
			result = true;
			return SRESULT_DONE;
		}
// crap...
		//if (check_blocked()) {
		//	result = true;
		//	return SRESULT_DONE;
		//}

		range = EnemyRange();
		if( !AI_ENEMY_VISIBLE || ( range > run_distance ) )
		{
			parms->subparam2 = true;
		}

		delta = idMath::AngleNormalize180( ideal_yaw - current_yaw );
		if( ( delta > walk_turn ) || ( delta < -walk_turn ) )
		{
			AI_RUN = false;
		}
		else
		{
			AI_RUN = parms->subparam2;
		}

		return SRESULT_WAIT;
	}

	result = true;
	return SRESULT_DONE;
}

/*
=====================
monster_base::check_blocked

returns true when an attack was called, since the move command may be different from when entering the function.
=====================
*/
stateResult_t idAI::check_blocked( stateParms_t* parms, bool& result )
{
	idEntity*	obstacle;
	float	attack_flags;
	idAI*	monster;
	float	endTime;

	if( parms->substage == 0 )
	{
		parms->subparam1 = AI_RUN;

		if( AI_BLOCKED )
		{
			//sys.print( sys.getTime() + " : " + getName() + " is stuck in place\n" );
			Event_SaveMove();
			AI_RUN = true;
			Event_Wander();
			parms->subparam2 = gameLocal.SysScriptTime() + 2;
			parms->substage = 1;
			return SRESULT_WAIT;
		}
		else if( move.moveStatus == MOVE_STATUS_BLOCKED_BY_OBJECT )
		{
			float force = GetFloatKey( "kick_force" );
			if( !force )
			{
				force = 60;
			}
			Event_KickObstacles( move.obstacle.GetEntity(), force );
			goto done;
		}
		else if( move.moveStatus > MOVE_STATUS_BLOCKED_BY_OBJECT )
		{
			// just wait for the path to be clear
			obstacle = move.obstacle.GetEntity();
			monster = ( idAI* )obstacle;
			if( monster )
			{
				if( monster->blocked )
				{
					goto done;
				}
			}
			blocked = true;
			Event_SaveMove();
			parms->subparam2 = 2;
			return SRESULT_WAIT;
		}
	}

	if( parms->substage == 1 )
	{
		if( gameLocal.SysScriptTime() < endTime )
		{
			attack_flags = check_attacks();
			if( attack_flags )
			{
				Event_RestoreMove();
				do_attack( attack_flags );
				AI_RUN = parms->subparam1;
				result = true;
				return SRESULT_DONE;
			}

			return SRESULT_WAIT;
		}
		Event_RestoreMove();
		goto done;
	}

	if( parms->substage == 2 )
	{
		if( move.moveStatus > MOVE_STATUS_BLOCKED_BY_OBJECT )
		{
			AI_RUN = true;
			Event_Wander();
			//	endTime = gameLocal.SysScriptTime() + 1;
			{
				if( AI_MOVE_DONE )
				{
					Event_FaceEnemy();
				}
				attack_flags = check_attacks();
				if( attack_flags )
				{
					blocked = false;
					Event_RestoreMove();
					do_attack( attack_flags );
					AI_RUN = parms->subparam1;
					result = true;
					return SRESULT_DONE;
				}
				//waitFrame();
			}
			Event_RestoreMove();
			return SRESULT_WAIT;
		}
		blocked = false;
	}

done:
	AI_RUN = parms->subparam1;
	result = false;
	return SRESULT_DONE;
}