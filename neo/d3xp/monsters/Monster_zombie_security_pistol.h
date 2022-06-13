// Monster_zombie_security_pistol.h
//

#pragma once

class rvmMonsterZombieSecurityPistol : public rvmMonsterZombie
{
	CLASS_PROTOTYPE( rvmMonsterZombieSecurityPistol );
public:
	virtual void				Init( void ) override;
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t state_Begin( stateParms_t* parms );
	stateResult_t state_Idle( stateParms_t* parms );
	stateResult_t stand_attack( stateParms_t* parms );
	stateResult_t crouch_attack( stateParms_t* parms );
	stateResult_t combat_dodge_right( stateParms_t* parms );
	stateResult_t combat_dodge_left( stateParms_t* parms );
private:
	idEntity*			combat_node;

	idScriptBool		fire;
	idScriptBool		crouch_fire;
	idScriptBool		run_attack;
	idScriptFloat		nextDodge;
	idScriptFloat		nextAttack;
	idScriptFloat		nextNoFOVAttack;

	// start out with a 50/50 chance of stand vs. crouch attacks.
	float zsecp_num_stand_attacks;
	float zsecp_num_crouch_attacks;
private:
	float attackTime;
};