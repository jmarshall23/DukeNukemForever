// Pigcop.h
//

#pragma once

enum PIGCOP_IDLE_STATE
{
	PIGCOP_IDLE_WAITINGTPLAYER = 0,
	PIGCOP_IDLE_ROAR
};

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