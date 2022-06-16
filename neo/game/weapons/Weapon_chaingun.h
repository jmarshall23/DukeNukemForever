// Weapon_chaingun.h
//

#pragma once

class rvmWeaponChainGun : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponChainGun );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	void		UpdateBarrel();
	void		SpinUp();
	void		SpinDown();
private:
	idAnimatedEntity*	world_model;
	jointHandle_t		world_barrel_joint;
	jointHandle_t		barrel_joint;
	float		barrel_angle;
	float		current_rate;
	float		start_rate;
	float		end_rate;
	float		spin_start;
	float		spin_end;
	float		spread;
	int			numSkipFrames;

	const idSoundShader*		snd_windup;
	const idSoundShader*		snd_winddown;
};