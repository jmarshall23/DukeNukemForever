// Pigcop.h
//

#pragma once

//
// DnPigcop
//
class DnPigcop : public DnAI
{
	CLASS_PROTOTYPE(DnPigcop);
public:
	stateResult_t				state_Begin(stateParms_t* parms);
	stateResult_t				state_Idle(stateParms_t* parms);
};