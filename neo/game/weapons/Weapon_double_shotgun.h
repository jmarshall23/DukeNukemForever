// Weapon_double_shotgun.h
//

#pragma once

class rvmWeaponDoubleShotgun : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponDoubleShotgun );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );

	void	EjectBrass( void );
private:

};