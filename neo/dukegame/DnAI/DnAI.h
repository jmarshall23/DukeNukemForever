// DnAI.h
//

#pragma once

class idPlayer;

//
// DnAI
//
class DnAI : public idActor
{
	CLASS_PROTOTYPE(DnAI);
public:
	void					Spawn(void);
	void					Think(void);
protected:
	void					SetAnimation(const char* anim, bool loop);

	idPlayer*				FindNewTarget();

protected:
	idActor*				target;
};

#include "Pigcop.h"