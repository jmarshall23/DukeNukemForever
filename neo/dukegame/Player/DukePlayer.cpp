// DukePlayer.cpp
//

#include "../../game/game_local.h"

const idEventDef EV_Player_DukeTalk("startDukeTalk", "s");

CLASS_DECLARATION(idPlayer, DukePlayer)
	EVENT(EV_Player_DukeTalk, DukePlayer::Event_DukeTalk)
END_CLASS

/*
==================
DukePlayer::DukePlayer
==================
*/
DukePlayer::DukePlayer()
{
	bob = 0.0f;
	lastAppliedBobCycle = 0.0f;
}

/*
==================
DukePlayer::UpdateHudStats
==================
*/
void DukePlayer::UpdateHudStats(idUserInterface* hud)
{
	hud->SetStateInt("player_ego", health);
	UpdateHudAmmo(hud);
}

/*
==================
DukePlayer::Event_DukeTalk
==================
*/
void DukePlayer::Event_DukeTalk(const char* soundName)
{
	if (gameLocal.IsParentalLockEnabled())
	{
		return;
	}

	gameSoundWorld->PlayShaderDirectly(soundName, SCHANNEL_DUKETALK);
}

/*
==================
DukePlayer::SetStartingInventory
==================
*/
void DukePlayer::SetStartingInventory(void)
{
	inventory.ammo[DN_WEAPON_PISTOL] = 48;

	Give("weapon", "weapon_mightyfoot");
	Give("weapon", "weapon_pistol");
	Give("weapon", "weapon_shotgun");
}

/*
===============
DukePlayer::ApplyLandDeflect
===============
*/
idVec3 DukePlayer::ApplyLandDeflect(const idVec3& pos, float scale) {
	idVec3	localPos(pos);
	int		delta = 0;
	float	fraction = 0.0f;

	// add fall height
	delta = gameLocal.time - landTime;
	if (delta < LAND_DEFLECT_TIME) {
		fraction = (float)delta / LAND_DEFLECT_TIME;
		fraction *= scale;
		localPos += -physicsObj.GetGravityNormal() * landChange * fraction;
	}
	else if (delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME) {
		fraction = (float)(LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
		fraction *= scale;
		localPos += -physicsObj.GetGravityNormal() * landChange * fraction;
	}

	return localPos;
}

/*
===============
DukePlayer::BobCycle
===============
*/
#define BOB_TO_NOBOB_RETURN_TIME 3000.0f
void DukePlayer::BobCycle(const idVec3& pushVelocity) {
	float		bobmove;
	int			old; //, deltaTime;
	idVec3		vel, gravityDir, velocity;
	idMat3		viewaxis;
	//	float		bob;
	float		delta;
	float		speed;
	//	float		f;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	velocity = physicsObj.GetLinearVelocity() - pushVelocity;

	gravityDir = physicsObj.GetGravityNormal();
	vel = velocity - (velocity * gravityDir) * gravityDir;
	xyspeed = vel.LengthFast();

	// do not evaluate the bob for other clients
	// when doing a spectate follow, don't do any weapon bobbing
	if (gameLocal.isClient && entityNumber != gameLocal.localClientNum) {
		bool canBob = false;
		if (gameLocal.localClientNum != -1 && gameLocal.entities[gameLocal.localClientNum] && gameLocal.entities[gameLocal.localClientNum]->IsType(DukePlayer::Type)) {
			DukePlayer* pl = static_cast<DukePlayer*>(gameLocal.entities[gameLocal.localClientNum]);
			if (pl->spectating && pl->spectator == entityNumber) {
				canBob = true;
			}
		}

		if (!canBob) {
			viewBobAngles.Zero();
			viewBob.Zero();
			return;
		}
	}

	if (!physicsObj.HasGroundContacts() || influenceActive == INFLUENCE_LEVEL2 || (gameLocal.isMultiplayer && spectating)) {
		// airborne
		bobCycle = 0;
		bobFoot = 0;
		bobfracsin = 0;
	}
	else if ((!usercmd.forwardmove && !usercmd.rightmove) || (xyspeed <= MIN_BOB_SPEED)) {
		// start at beginning of cycle again
		bobCycle = 0;
		bobFoot = 0;
		bobfracsin = 0;
	}
	else {
		if (physicsObj.IsCrouching()) {
			bobmove = pm_crouchbob.GetFloat();
			// ducked characters never play footsteps
		}
		else {
			// vary the bobbing based on the speed of the player
			bobmove = pm_walkbob.GetFloat() * (1.0f - bobFrac) + pm_runbob.GetFloat() * bobFrac;
		}

		// check for footstep / splash sounds
		old = bobCycle;
		bobCycle = (int)(old + bobmove * gameLocal.msec) & 255;
		bobFoot = (bobCycle & 128) >> 7;
		bobfracsin = idMath::Fabs(sin((bobCycle & 127) / 127.0 * idMath::PI));
	}

	// calculate angles for view bobbing
	viewBobAngles.Zero();

	viewaxis = viewAngles.ToMat3();

	// add angles based on velocity
	delta = velocity * viewaxis[0];
	viewBobAngles.pitch += delta * pm_runpitch.GetFloat();

	delta = velocity * viewaxis[1];
	viewBobAngles.roll -= delta * pm_runroll.GetFloat();

	// add angles based on bob
	// make sure the bob is visible even at low speeds
	speed = xyspeed > 200 ? xyspeed : 200;

	delta = bobfracsin * pm_bobpitch.GetFloat() * speed;
	if (physicsObj.IsCrouching()) {
		delta *= 3;		// crouching
	}
	viewBobAngles.pitch += delta;
	delta = bobfracsin * pm_bobroll.GetFloat() * speed;
	if (physicsObj.IsCrouching()) {
		delta *= 3;		// crouching accentuates roll
	}
	if (bobFoot & 1) {
		delta = -delta;
	}
	viewBobAngles.roll += delta;

	{
		idVec3 pos = ApplyLandDeflect(vec3_zero, 1.0f);

		float			delta = 0.0f;
		idVec3			localViewBob(pos);
		const float		maxBob = 6.0f;

		if (bob > 0.0f && !usercmd.forwardmove && !usercmd.rightmove) {//smoothly goto bob of zero if not already there
			delta = gameLocal.time - lastAppliedBobCycle;
			bob *= (1.0f - (delta / BOB_TO_NOBOB_RETURN_TIME));
			if (bob < 0.0f) {
				bob = 0.0f;
			}
		}
		else {
			// add bob height after any movement smoothing
			lastAppliedBobCycle = gameLocal.time;
			bob = bobfracsin * xyspeed * pm_bobup.GetFloat();
			if (bob > maxBob) {
				bob = maxBob;
			}
		}

		localViewBob -= physicsObj.GetGravityNormal() * bob;

		viewBob = localViewBob;
	}
}