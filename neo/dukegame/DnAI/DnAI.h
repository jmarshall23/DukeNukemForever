// DnAI.h
//

#pragma once

class idPlayer;

//
// DnRand 
// Basically a wrapper for krand from Duke3D.
//
class DnRand
{
public:
	DnRand()
	{
		randomseed = 17;
	}

	unsigned int GetRand()
	{
		randomseed = (randomseed * 27584621) + 1;
		return(((unsigned int)randomseed) >> 16);
	}

	bool ifrnd(int val1)
	{
		return (((GetRand() >> 8) >= (255 - (val1))));
	}
private:
	int randomseed;
};

extern DnRand dnRand;

//
// DnAI
//
class DnAI : public idActor
{
	CLASS_PROTOTYPE(DnAI);
public:
	void					Spawn(void);
	void					Think(void);

	bool					CurrentlyPlayingSound();

	bool					FacingIdeal();
	bool					TurnToward(float yaw);
	bool					TurnToward(const idVec3& pos);

	bool					IsOnGround() { return AI_ONGROUND; }
	bool					MoveToPosition(const idVec3& pos);
protected:
	void					SetAnimation(const char* anim, bool loop);

	void					UpdatePathToPosition(idVec3 position);

	idPlayer*				FindNewTarget();

	void					StopMove(moveStatus_t status);

protected:
	idActor*				target;
	idVec3					targetLastSeenLocation;
	bool					isTargetVisible;

	idPhysics_Monster		physicsObj;

	float					ideal_yaw;
	float					current_yaw;
	float					turnRate;
	float					turnVel;


	idMoveState				move;

	idList<idVec3>			pathWaypoints;
	int						waypointId;
private:
	void					Turn();
	void					SetupPhysics(void);
	void					SlideMove();
	bool					ReachedPos(const idVec3& pos, const moveCommand_t moveCommand) const;

	bool					AI_ONGROUND;
	bool					AI_BLOCKED;

	idStr					currentAnimation;
};

#include "Pigcop.h"