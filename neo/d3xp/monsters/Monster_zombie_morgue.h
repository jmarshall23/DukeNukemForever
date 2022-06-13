// Monster_zombie_morgue.h
//

#pragma once

class rvmMonsterZombieMorgue : public rvmMonsterZombie
{
	CLASS_PROTOTYPE( rvmMonsterZombieMorgue );
public:
	virtual void				Init( void ) override;
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_melee( stateParms_t* parms );

};
