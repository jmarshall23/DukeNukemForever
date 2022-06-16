// Monster_zombie_bernie.h
//

#pragma once

class rvmMonsterZombieBernie : public rvmMonsterZombie
{
	CLASS_PROTOTYPE( rvmMonsterZombieBernie );
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