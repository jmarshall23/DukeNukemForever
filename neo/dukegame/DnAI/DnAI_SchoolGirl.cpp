// DnAI_SchoolGirl.cpp
//

#include "../../game/Game_local.h"

CLASS_DECLARATION(DnAI, DnSchoolGirl)
END_CLASS

/*
===================
DnSchoolGirl::state_Begin
===================
*/
stateResult_t DnSchoolGirl::state_Begin(stateParms_t* parms)
{
	SetAnimation("idle", false);
	Event_SetState("state_Idle");
	return SRESULT_DONE;
}

/*
===================
DnSchoolGirl::state_Idle
===================
*/
stateResult_t DnSchoolGirl::state_Idle(stateParms_t* parms)
{
	return SRESULT_WAIT;
}