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
	SetAnimation("idle");

	Event_SetState("state_Idle");
	return SRESULT_DONE;
}

/*
==============
DnPigcop::state_Idle
==============
*/
stateResult_t DnPigcop::state_Idle(stateParms_t* parms)
{
	return SRESULT_WAIT;
}