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

	return SRESULT_DONE;
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
				SetAnimation("roar", false);
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
	}

	return SRESULT_WAIT;
}