// Weapon_grabber.h
//

#pragma once

class rvmWeaponGrabber : public rvmWeaponObject
{
public:
	CLASS_PROTOTYPE( rvmWeaponGrabber );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	void		StartActive();
	void		StopActive();

	void		StartWarningSound();
	void		UpdateWarningSound();

	void		UpdateGuiLight();

	const idSoundShader* snd_fireloop;
	const idSoundShader* snd_electroloop;
	const idSoundShader* snd_mainfire;
	const idSoundShader* snd_cangrab;
	const idSoundShader* snd_warning;
	const idSoundShader* snd_stopfire;
private:
	float		next_attack;

	float		fireStartTime;
	bool		warningBeep1;
	bool		warningBeep2;
	bool		warningBeep3;
	bool		warningBeep4;

	float		grabberState;
};