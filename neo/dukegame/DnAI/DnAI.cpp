// DnAI.cpp
//

#include "precompiled.h"
#include "../../game/Game_local.h"

CLASS_DECLARATION(idActor, DnAI)
END_CLASS

DnRand dnRand;

/*
===============
DnAI::Spawn
===============
*/
void DnAI::Spawn(void)
{
	target = nullptr;

	idActor::Spawn();

	stateThread.SetOwner(this);

	BecomeActive(TH_THINK);

	SetState("state_Begin");
}

/*
===============
DnAI::FindNewTarget
===============
*/
idPlayer* DnAI::FindNewTarget()
{
	idPlayer* localPlayer = gameLocal.GetLocalPlayer();

	if (CanSee(localPlayer, true))
	{
		return localPlayer;
	}

	return nullptr;
}


/*
===============
DnAI::SetAnimation
===============
*/
void DnAI::SetAnimation(const char* name, bool loop)
{
	int				animNum;
	int anim;
	const idAnim* newanim;

	animNum = animator.GetAnim(name);

	if (!animNum) {
		gameLocal.Printf("Animation '%s' not found.\n", name);
		return;
	}

	anim = animNum;
	//starttime = gameLocal.time;
	//animtime = animator.AnimLength(anim);
	//headAnim = 0;

	if (loop)
	{
		animator.CycleAnim(ANIMCHANNEL_ALL, anim, gameLocal.time, 0);
	}
	else
	{
		animator.PlayAnim(ANIMCHANNEL_ALL, anim, gameLocal.time, 0);
	}
	animator.RemoveOriginOffset(true);
}

bool DnAI::CurrentlyPlayingSound()
{
	if (refSound.referenceSound == nullptr)
		return false;

	return refSound.referenceSound->CurrentlyPlaying();
}

void DnAI::Think(void)
{
	idActor::Think();
	stateThread.Execute();
}