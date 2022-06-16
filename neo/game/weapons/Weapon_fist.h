// Weapon_fist.h
//

#pragma once

class rvmWeaponFist : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponFist );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	const char* 			GetFireAnim();
	bool side;
};