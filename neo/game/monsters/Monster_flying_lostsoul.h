// Monster_flying_lostsoul.h
//

#pragma once

class rvmMonsterLostSoul : public idAI
{
	CLASS_PROTOTYPE( rvmMonsterLostSoul );
public:
	virtual void				Init( void ) override;
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_charge( stateParms_t* parms );
	stateResult_t				combat_melee( stateParms_t* parms );
	stateResult_t				combat_retreat( stateParms_t* parms );

	stateResult_t				Torso_Idle(stateParms_t* parms);
	stateResult_t				Torso_Fly(stateParms_t* parms);
	stateResult_t				Torso_Charge(stateParms_t* parms);
private:
	float		nextAttack;
	float		nextNoFOVAttack;
	float		noMeleeTime;
	float		fly_offset;
private:
	idVec3		vel;
	idVec3		pos;
	float		endtime;
};
