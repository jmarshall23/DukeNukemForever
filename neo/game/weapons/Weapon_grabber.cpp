// Weapon_grabber.cpp
//


#pragma hdrstop
#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION( rvmWeaponObject, rvmWeaponGrabber )
END_CLASS

// blend times
#define GRABBER_IDLE_TO_LOWER		4
#define GRABBER_IDLE_TO_OPEN		4
#define GRABBER_IDLE_TO_OPENFIRE	4
#define GRABBER_OPEN_TO_CLOSE		4
#define GRABBER_OPEN_TO_FIRE		4
#define GRABBER_CLOSE_TO_OPEN		4
#define GRABBER_CLOSE_TO_IDLE		4
#define GRABBER_OPENFIRE_TO_FIRE	2
#define GRABBER_FIRE_TO_CLOSE		4
#define GRABBER_RAISE_TO_IDLE		4

#define WARNING_BEEP_1				1.5
#define WARNING_BEEP_2				2.25
#define WARNING_BEEP_3				2.50
#define WARNING_BEEP_4				2.75

/*
===============
rvmWeaponGrabber::Init
===============
*/
void rvmWeaponGrabber::Init( idWeapon* weapon )
{
	rvmWeaponObject::Init( weapon );

	next_attack = 0;

	snd_fireloop = FindSound( "snd_fireloop" );
	snd_electroloop = FindSound( "snd_electroloop" );
	snd_mainfire = FindSound( "snd_mainfire" );
	snd_cangrab = FindSound( "snd_cangrab" );
	snd_warning = FindSound( "snd_warning" );
	snd_stopfire = FindSound( "snd_stopfire" );

	owner->Event_Grabber( true );

	grabberState = -1;
}


/*
===============
rvmWeaponGrabber::Raise
===============
*/
stateResult_t rvmWeaponGrabber::Raise( stateParms_t* parms )
{
	enum RisingState
	{
		RISING_NOTSET = 0,
		RISING_WAIT
	};

	switch( parms->stage )
	{
		case RISING_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "raise", false );
			parms->stage = RISING_WAIT;
			return SRESULT_WAIT;

		case RISING_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, GRABBER_RAISE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}


/*
===============
rvmWeaponGrabber::Lower
===============
*/
stateResult_t rvmWeaponGrabber::Lower( stateParms_t* parms )
{
	enum LoweringState
	{
		LOWERING_NOTSET = 0,
		LOWERING_WAIT
	};

	switch( parms->stage )
	{
		case LOWERING_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "putaway", false );
			parms->stage = LOWERING_WAIT;
			return SRESULT_WAIT;

		case LOWERING_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				SetState( "Holstered" );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}
/*
===============
rvmWeaponGrabber::CanFire

Original grabber logic had this, so we prevent the fire action from going unless grabState is 1 or 2.

grabState = grabberHasTarget();
if ( grabState == 1 || grabState == 2 ) {
	if(WEAPON_ATTACK) {
		weaponState( "GrabberOpenFire", GRABBER_IDLE_TO_OPENFIRE );
	}
}
===============
*/
//bool rvmWeaponGrabber::CanFire()
//{
//	int grabState = owner->Event_GrabberHasTarget();
//	return grabState == 1 || grabState == 2;
//}

/*
===============
rvmWeaponGrabber::Idle
===============
*/
stateResult_t rvmWeaponGrabber::Idle( stateParms_t* parms )
{
	int grabState = 0;

	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT,
		IDLE_GRABBER_OPEN,
		IDLE_GRABBER_OPEN_WAIT,
	};

	UpdateGuiLight();

	grabState = owner->Event_GrabberHasTarget();
	if( grabState == 1 || grabState == 2 )
	{
		parms->stage = IDLE_GRABBER_OPEN;
	}

	switch( parms->stage )
	{
		case IDLE_NOTSET:
			owner->Event_WeaponReady();
			if( !owner->AmmoInClip() )
			{
				owner->Event_WeaponOutOfAmmo();
			}
			else
			{
				owner->Event_WeaponReady();
			}

			owner->Event_PlayCycle( ANIMCHANNEL_ALL, "idle" );
			parms->stage = IDLE_WAIT;
			return SRESULT_WAIT;

		case IDLE_WAIT:
			// Do nothing.
			return SRESULT_WAIT;

		case IDLE_GRABBER_OPEN:
			owner->StartSoundShader( snd_fireloop, SND_CHANNEL_BODY3, 0, false, NULL );
			owner->StartSoundShader( snd_electroloop, SND_CHANNEL_BODY2, 0, false, NULL );
			owner->StartSoundShader( snd_mainfire, SND_CHANNEL_BODY, 0, false, NULL );

			StartWarningSound();
			StartActive();

			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "open", false );
			parms->stage = IDLE_GRABBER_OPEN_WAIT;
			return SRESULT_WAIT;

		case IDLE_GRABBER_OPEN_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				//owner->WeaponState( WP_FIRE, GRABBER_OPEN_TO_CLOSE );
				owner->BeginAttack();
				return SRESULT_DONE;
			}
			else
			{
				grabState = owner->Event_GrabberHasTarget();
				if( grabState == 3 )
				{
					//owner->WeaponState( WP_FIRE, GRABBER_OPEN_TO_CLOSE );
					owner->BeginAttack();
					return SRESULT_DONE;
				}
			}

			UpdateGuiLight();
			UpdateWarningSound();
			return SRESULT_WAIT;;
	}

	return SRESULT_DONE;
}

/*
=====================
rvmWeaponGrabber::StartWarningSound
=====================
*/
void rvmWeaponGrabber::StartWarningSound()
{
	fireStartTime = gameLocal.realClientTime;
	warningBeep1 = false;
	warningBeep2 = false;
	warningBeep3 = false;
	warningBeep4 = false;
}

/*
=====================
rvmWeaponGrabber::StartActive
=====================
*/
void rvmWeaponGrabber::StartActive()
{
	owner->Event_StartWeaponParticle( "barrel_upper" );
	owner->Event_StartWeaponLight( "light_barrel_upper" );
	owner->Event_StartWeaponLight( "light_barrel_lower" );
	owner->Event_StartWeaponLight( "light_side" );
}

/*
=====================
rvmWeaponGrabber::StopActive
=====================
*/
void rvmWeaponGrabber::StopActive()
{
	owner->Event_StopWeaponParticle( "barrel_upper" );

	owner->Event_StopWeaponLight( "light_barrel_upper" );
	owner->Event_StopWeaponLight( "light_barrel_lower" );
	owner->Event_StopWeaponLight( "light_side" );
}

/*
==================== =
rvmWeaponGrabber::UpdateGuiLight
==================== =
*/
void rvmWeaponGrabber::UpdateGuiLight()
{

	float newState = owner->Event_GrabberHasTarget();
	if( newState != grabberState )
	{
		grabberState = newState;
		if( grabberState == 0 )
		{
			owner->Event_StartWeaponLight( "gLightBlue" );
			owner->Event_StopWeaponLight( "gLightYellow" );
			owner->Event_StopWeaponLight( "gLightRed" );
		}
		else if( grabberState == 1 )
		{
			//startSound("snd_cangrab", SND_CHANNEL_ITEM, false);
			owner->StartSoundShader( snd_cangrab, SND_CHANNEL_ITEM, 0, false, NULL );
			owner->Event_StartWeaponLight( "gLightYellow" );
			owner->Event_StopWeaponLight( "gLightBlue" );
			owner->Event_StopWeaponLight( "gLightRed" );
		}
		else if( grabberState == 2 )
		{
			owner->Event_StartWeaponLight( "gLightRed" );
			owner->Event_StopWeaponLight( "gLightBlue" );
			owner->Event_StopWeaponLight( "gLightYellow" );
		}
	}
}

/*
=====================
rvmWeaponGrabber::UpdateWarningSound
=====================
*/
void rvmWeaponGrabber::UpdateWarningSound()
{
	float currentTime;
	float elapsed;

	currentTime = gameLocal.realClientTime;
	elapsed = currentTime - fireStartTime;

	if( elapsed > WARNING_BEEP_1 && !warningBeep1 )
	{
		owner->StartSoundShader( snd_warning, SND_CHANNEL_ITEM, 0, false, NULL );
		warningBeep1 = true;
	}
	if( elapsed > WARNING_BEEP_2 && !warningBeep2 )
	{
		owner->StartSoundShader( snd_warning, SND_CHANNEL_ITEM, 0, false, NULL );
		warningBeep2 = true;
	}
	if( elapsed > WARNING_BEEP_3 && !warningBeep3 )
	{
		owner->StartSoundShader( snd_warning, SND_CHANNEL_ITEM, 0, false, NULL );
		warningBeep3 = true;
	}
	if( elapsed > WARNING_BEEP_4 && !warningBeep4 )
	{
		owner->StartSoundShader( snd_warning, SND_CHANNEL_ITEM, 0, false, NULL );
		warningBeep4 = true;
	}
}


/*
===============
rvmWeaponGrabber::Fire
===============
*/
stateResult_t rvmWeaponGrabber::Fire( stateParms_t* parms )
{
	//int grabState;
	//
	//grabState = owner->Event_GrabberHasTarget();
	//
	//enum FIRE_State
	//{
	//	FIRE_NOTSET = 0,
	//	FIRE_WAIT_GRABSTATE,
	//	FIRE_ACTUALLY_FIRE,
	//	FIRE_WAIT
	//};
	//
	//switch( parms->stage )
	//{
	//	case FIRE_NOTSET:
	//		next_attack = MS2SEC( gameLocal.realClientTime );
	//		owner->Event_PlayAnim( ANIMCHANNEL_ALL, "idleopen", true );
	//		parms->stage = FIRE_WAIT_GRABSTATE;
	//		return SRESULT_WAIT;
	//
	//	case FIRE_WAIT_GRABSTATE:
	//		if( grabState == 3 || grabState == 0 )
	//		{
	//			parms->stage = FIRE_ACTUALLY_FIRE;
	//		}
	//		return SRESULT_WAIT;
	//
	//	case FIRE_ACTUALLY_FIRE:
	//		StopActive();
	//
	//		// Stops fire loop sound
	//		owner->StartSoundShader( snd_stopfire, SND_CHANNEL_BODY3, 0, false, NULL );
	//		owner->StopSound( SND_CHANNEL_BODY2, false );
	//
	//		owner->Event_StartWeaponSmoke();
	//		owner->Event_PlayAnim( ANIMCHANNEL_ALL, "fire", true );
	//		break;
	//
	//	case FIRE_WAIT:
	//		if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
	//		{
	//			owner->WeaponState( WP_IDLE, GRABBER_CLOSE_TO_IDLE );
	//			firingState = 0;
	//		}
	//		else
	//		{
	//			UpdateGuiLight();
	//		}
	//		break;
	//}
	return SRESULT_DONE;
}

/*
===============
rvmWeaponGrabber::Reload
===============
*/
stateResult_t rvmWeaponGrabber::Reload( stateParms_t* parms )
{
	return SRESULT_DONE;
}