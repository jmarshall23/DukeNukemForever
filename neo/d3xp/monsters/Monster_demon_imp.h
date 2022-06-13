// Monster_demon_imp.h
//

#pragma once

class rvmMonsterDemonImp : public idAI
{
	CLASS_PROTOTYPE( rvmMonsterDemonImp );
public:
	virtual void				Init( void ) override;
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_melee( stateParms_t* parms );
	stateResult_t				combat_leap( stateParms_t* parms );
	stateResult_t				combat_range( stateParms_t* parms );
	stateResult_t				combat_dodge_left( stateParms_t* parms );
	stateResult_t				combat_dodge_right( stateParms_t* parms );
private:
	float		nextDodge;
	float		nextAttack;
	float		nextLeap;
	float		nextNoFOVAttack;
	idEntity*	combat_node;
	idStr range_attack_anim;
	idVec3 jumpVelocity;
private:
	stateResult_t				Torso_LeapAttack(stateParms_t* parms);
	stateResult_t				Legs_Idle(stateParms_t* parms);
	stateResult_t				Legs_DodgeLeft(stateParms_t* parms);
	stateResult_t				Legs_DodgeRight(stateParms_t* parms);
};