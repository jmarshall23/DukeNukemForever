// Monster_zombie_sawyer.h
//

#pragma once

class rvmMonsterZombieSawyer : public rvmMonsterZombie
{
	CLASS_PROTOTYPE( rvmMonsterZombieSawyer );
public:
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_melee( stateParms_t* parms );

	float next_hit_time;
	int smoke_frames;
};
