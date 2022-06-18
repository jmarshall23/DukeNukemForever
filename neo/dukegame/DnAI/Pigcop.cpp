// Pigcop.cpp
//

#include "precompiled.h"
#include "../../game/Game_local.h""

CLASS_DECLARATION(DnAI, DnPigcop)
END_CLASS

/*
==============
DnPigcop::state_Begin
==============
*/
stateResult_t DnPigcop::state_Begin(stateParms_t* parms)
{
	SetAnimation("idle", false);

	Event_SetState("state_Idle");

	pig_roam1 = declManager->FindSound("pig_roam1", false);
	pig_roam2 = declManager->FindSound("pig_roam2", false);
	pig_roam3 = declManager->FindSound("pig_roam3", false);
	pig_awake = declManager->FindSound("pig_awake", false);

	return SRESULT_DONE;
}

/*
==============
DnPigcop::state_ApproachingEnemy
==============
*/
stateResult_t DnPigcop::state_ApproachingEnemy(stateParms_t* parms)
{
	float distToEnemy = 0.0f;

	distToEnemy = (target->GetOrigin() - GetOrigin()).Length();

	if (distToEnemy > 600)
	{
		idList<idVec3> waypoints;
		if (!gameLocal.GetNavigation()->GetPathBetweenPoints(GetOrigin(), target->GetOrigin(), waypoints))
		{
			common->Warning("Failed to get path between self and target!");
			return SRESULT_WAIT;
		}

	//	MoveToPosition(waypoints[0]);
	}
	

	return SRESULT_WAIT;
}

/*
==============
DnPigcop::state_Idle
==============
*/
stateResult_t DnPigcop::state_Idle(stateParms_t* parms)
{
	switch (parms->stage)
	{
		case PIGCOP_IDLE_WAITINGTPLAYER:
			target = FindNewTarget();

			if (target != nullptr)
			{
				targetLastSeenLocation = target->GetOrigin();
				isTargetVisible = true;

				SetAnimation("roar", false);
				StartSoundShader(pig_awake, SND_CHANNEL_ANY, 0, false, nullptr);
				parms->stage = PIGCOP_IDLE_ROAR;
			}
			else
			{
				if (!CurrentlyPlayingSound())
				{
					if (dnRand.ifrnd(1))
					{
						if (dnRand.ifrnd(32))
						{
							StartSoundShader(pig_roam1, SND_CHANNEL_ANY, 0, false, nullptr);
						}
						else
						{
							if (dnRand.ifrnd(64))
							{
								StartSoundShader(pig_roam2, SND_CHANNEL_ANY, 0, false, nullptr);
							}
							else
							{
								StartSoundShader(pig_roam3, SND_CHANNEL_ANY, 0, false, nullptr);
							}
						}
					}
					
				}
			}
			break;

		case PIGCOP_IDLE_ROAR:
			Event_SetState("state_ApproachingEnemy");
			return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}