// DnAI_Liztroop.cpp
//


#include "../../game/Game_local.h"

#define LIZTROOP_FIRE_DISTANCE			500

CLASS_DECLARATION(DnAI, DnLiztroop)
END_CLASS

/*
==============
DnLiztroop::state_Begin
==============
*/
stateResult_t DnLiztroop::state_Begin(stateParms_t* parms)
{
	SetAnimation("idle", false);

	Event_SetState("state_Idle");

	troop_awake = declManager->FindSound("liztroop_awake", false);
	fire_sound = declManager->FindSound("liztroop_fire", false);
	death_sound = declManager->FindSound("liztroop_die", false);

	return SRESULT_DONE;
}

/*
==============
DnLiztroop::state_BeginDeath
==============
*/
stateResult_t DnLiztroop::state_BeginDeath(stateParms_t* parms)
{
	Event_SetState("state_Killed");

	StopMove();

	SetAnimation("death", false);

	StartSoundShader(death_sound, SND_CHANNEL_ANY, 0, false, nullptr);

	return SRESULT_DONE;
}

/*
==============
DnLiztroop::state_Killed
==============
*/
stateResult_t DnLiztroop::state_Killed(stateParms_t* parms)
{
	return SRESULT_WAIT; // Were dead so do nothing.
}

/*
==============
DnLiztroop::state_ShootEnemy
==============
*/
stateResult_t DnLiztroop::state_ShootEnemy(stateParms_t* parms)
{
	// If we are firing, don't make any new decisions until its done.
	if ((animator.IsAnimating(gameLocal.time) || CurrentlyPlayingSound()) && GetCurrentAnimation() == "fire")
	{
		TurnToward(target->GetOrigin());
		animator.RemoveOriginOffset(true);
		return SRESULT_WAIT;
	}

	float distToEnemy = 0.0f;
	distToEnemy = (target->GetOrigin() - GetOrigin()).Length();

	if (distToEnemy < LIZTROOP_FIRE_DISTANCE + 25)
	{
		if (!isTargetVisible)
		{
			Event_SetState("state_ApproachingEnemy");
			return SRESULT_DONE;
		}

		TurnToward(target->GetOrigin());
		ResetAnimation();
		SetAnimation("fire", false);
		StartSoundShader(fire_sound, SND_CHANNEL_ANY, 0, false, nullptr);

		idVec3 muzzleOrigin = GetOrigin() + idVec3(0, 0, 40);
		idVec3 muzzleDir = muzzleOrigin - (target->GetOrigin() + target->GetVisualOffset());

		muzzleDir.Normalize();
		Hitscan(muzzleOrigin, -muzzleDir, 1, 1, 10);

		return SRESULT_WAIT;
	}
	else
	{
		Event_SetState("state_ApproachingEnemy");
		return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}

/*
==============
DnLiztroop::state_ApproachingEnemy
==============
*/
stateResult_t DnLiztroop::state_ApproachingEnemy(stateParms_t* parms)
{
	float distToEnemy = 0.0f;

	distToEnemy = (target->GetOrigin() - GetOrigin()).Length();

	if (distToEnemy > LIZTROOP_FIRE_DISTANCE || !isTargetVisible)
	{
		UpdatePathToPosition(target->GetOrigin());
		SetAnimation("walk", true);
	}
	else
	{
		StopMove();
		Event_SetState("state_ShootEnemy");
		return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}

/*
==============
DnLiztroop::state_Idle
==============
*/
stateResult_t DnLiztroop::state_Idle(stateParms_t* parms)
{
	switch (parms->stage)
	{
		case LIZTROOP_IDLE_WAITINGTPLAYER:
			target = FindNewTarget();

			if (target != nullptr)
			{
				targetLastSeenLocation = target->GetOrigin();
				isTargetVisible = true;

				StartSoundShader(troop_awake, SND_CHANNEL_ANY, 0, false, nullptr);
				parms->stage = LIZTROOP_IDLE_ROAR;
			}
			break;

		case LIZTROOP_IDLE_ROAR:
			Event_SetState("state_ApproachingEnemy");
			return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}