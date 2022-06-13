// Monster_boss_vagary.h
//

#pragma once


class rvmMonsterBossVagary : public idAI
{
public:
	CLASS_PROTOTYPE( rvmMonsterBossVagary );
	virtual void				Init( void ) override;
	virtual void				AI_Begin( void ) override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	idEntity* ChooseObjectToThrow( const idVec3& mins, const idVec3& maxs, float speed, float minDist, float offset );
	void	ThrowObjectAtEnemy( idEntity* ent, float speed );
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_dodge_right( stateParms_t* parms );
	stateResult_t				combat_dodge_left( stateParms_t* parms );
	stateResult_t				combat_melee( stateParms_t* parms );
	stateResult_t				combat_range( stateParms_t* parms );
private:
	float		nextDodge;
	float		nextAttack;
	float		nextNoFOVAttack;
	idEntity*	combat_node;
	idEntity*	throwEntity;
	int num;
	int i;
	idVec3	pos;
	float	waitTime;
	float	t;
	idVec3	offset;
	idVec3	vel;
	float	start_offset;
};