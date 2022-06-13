// Weapon_handgrenade.h
//

#pragma once

class rvmWeaponHandgrenade : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponHandgrenade );

	virtual void			Init( idWeapon* weapon );
	virtual void			OwnerDied( void ) override;

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	void					GrenadeNade( void );
	void					GrenadeNoNade( void );
	void					UpdateSkin();

	void					ExplodeInHand();
private:
	float		spread;
	float		fuse_start;
	idStr		skin_nade;
	idStr		skin_nade_invis;
	idStr		skin_nonade;
	idStr		skin_nonade_invis;
	idProjectile* projectile;

	boolean		show_grenade;
private:
	float fuse_end;
	float current_time;
	float time_held;
	float power;
	boolean exploded;


	const idSoundShader* snd_lowammo;
};