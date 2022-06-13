// Monster_zombie_commando_cgun.h
//

#pragma once

//
// rvmMonsterZombieCommandoChaingun
//
class rvmMonsterZombieCommandoChaingun : public idAI
{
	CLASS_PROTOTYPE( rvmMonsterZombieCommandoChaingun );
public:
	virtual void				Init( void ) override;
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_dodge_right( stateParms_t* parms );
	stateResult_t				combat_dodge_left( stateParms_t* parms );
	stateResult_t				crouch_attack( stateParms_t* parms );
	stateResult_t				stand_attack( stateParms_t* parms );

	stateResult_t				Torso_Idle(stateParms_t* parms);
	stateResult_t				Torso_RangeAttack(stateParms_t* parms);
	//stateResult_t				Torso_CrouchAttack(stateParms_t* parms);
private:
	bool		fire;
	bool		crouch_fire;
	bool		step_left;
	bool		step_right;
	float		nextDodge;
	float		nextAttack;
	float		nextNoFOVAttack;
private:
	idEntity*			combat_node;
	float				attackTime;
};
