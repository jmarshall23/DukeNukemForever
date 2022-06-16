// Weapon_flashlight.h
//

#pragma once

class rvmWeaponFlashlight : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponFlashlight );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	void					UpdateSkin( void );
	void					UpdateLightIntensity( void );

	bool					on;
	float					intensity;
	idStr					skin_on;
	idStr					skin_on_invis;
	idStr					skin_off;
	idStr					skin_off_invis;
};
