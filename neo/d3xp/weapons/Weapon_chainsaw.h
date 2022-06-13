// Weapon_chainsaw.h
//

#pragma once

class rvmWeaponChainsaw : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponChainsaw );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	bool side;
};
