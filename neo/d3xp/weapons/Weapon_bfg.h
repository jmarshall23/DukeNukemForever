// Weapon_bfg.h
//

#pragma once

class rvmWeaponBFG : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponBFG );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	void					OverCharge();

	float					spread;

	float					fuse_start;
	float					fuse_end;
	float					powerLevel;
	float					fire_time;
};