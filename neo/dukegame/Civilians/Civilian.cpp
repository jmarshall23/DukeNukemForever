// Civilian.cpp
//

#include "../../game/Game_local.h"

CLASS_DECLARATION(DnAI, DnCivilian)
END_CLASS

/*
==============
DnCivilian::state_Begin
==============
*/
stateResult_t DnCivilian::state_Begin(stateParms_t* parms)
{
	SetAnimation("idle", true);
	Event_SetState("state_Idle");

	return SRESULT_DONE;
}

/*
==============
DnCivilian::state_Idle
==============
*/
stateResult_t DnCivilian::state_Idle(stateParms_t* parms)
{
	return SRESULT_WAIT;
}