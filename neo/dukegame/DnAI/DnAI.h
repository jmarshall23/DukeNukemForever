// DnAI.h
//

#pragma once

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
	void					SetAnimation(const char* anim);
};

#include "Pigcop.h"