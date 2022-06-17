// DnAI.cpp
//

#include "precompiled.h"
#include "../../game/Game_local.h"

CLASS_DECLARATION(idActor, DnAI)
END_CLASS

/*
===============
DnAI::Spawn
===============
*/
void DnAI::Spawn(void)
{
	idActor::Spawn();

	stateThread.SetOwner(this);

	BecomeActive(TH_THINK);

	SetState("state_Begin");
}

/*
===============
DnAI::SetAnimation
===============
*/
void DnAI::SetAnimation(const char* name)
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

	animator.CycleAnim(ANIMCHANNEL_ALL, anim, gameLocal.time, 0);
	animator.RemoveOriginOffset(true);
}

void DnAI::Think(void)
{
	idActor::Think();
	stateThread.Execute();
}