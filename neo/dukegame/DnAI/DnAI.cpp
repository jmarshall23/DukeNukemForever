// DnAI.cpp
//


#include "../../game/Game_local.h"

CLASS_DECLARATION(idActor, DnAI)
END_CLASS

DnRand dnRand;

/*
===============
DnAI::Spawn
===============
*/
void DnAI::Spawn(void)
{
	target = nullptr;
	isTargetVisible = false;
	ideal_yaw = 0;
	current_yaw = 0;
	turnRate = 0;
	turnVel = 0;
	AI_ONGROUND = false;
	AI_BLOCKED = false;
	startedDeath = false;

	fl.takedamage = true;

	spawnArgs.GetFloat("turn_rate", "360", turnRate);

	EgoKillValue = spawnArgs.GetInt("EgoKillValue", "0");
	health = spawnArgs.GetInt("Health", "10");

	idActor::Spawn();

	stateThread.SetOwner(this);

	SetupPhysics();

	BecomeActive(TH_THINK);

	eyeOffset.z = 50.0f;

	SetState("state_Begin");
}

/*
===============
DnAI::SetupPhysics
===============
*/
void DnAI::SetupPhysics(void)
{
	idVec3				local_dir;

	fl.solidForTeam = true;

	idVec3 spawnOrigin = GetOrigin();
	idClipModel* clipModel = new idClipModel();

	idVec3 offset = spawnArgs.GetVector("model_offset", "0 0 0");

	physicsObj.SetSelf(this);
	idClipModel* newClip = new idClipModel(idTraceModel(renderEntity.hModel->Bounds() + offset));
	newClip->Translate(spawnOrigin);
	physicsObj.SetClipModel(newClip, 1.0f);
	physicsObj.SetMass(spawnArgs.GetFloat("mass", "100"));

	physicsObj.SetContents(CONTENTS_BODY | CONTENTS_SOLID);
	physicsObj.SetClipMask(MASK_MONSTERSOLID);

	// move up to make sure the monster is at least an epsilon above the floor
	physicsObj.SetOrigin(spawnOrigin + idVec3(0, 0, CM_CLIP_EPSILON));

	idVec3 gravity = spawnArgs.GetVector("gravityDir", "0 0 -1");
	gravity *= g_gravity.GetFloat();
	physicsObj.SetGravity(gravity);

	SetPhysics(&physicsObj);

	physicsObj.GetGravityAxis().ProjectVector(viewAxis[0], local_dir);
	current_yaw = local_dir.ToYaw();
	ideal_yaw = idMath::AngleNormalize180(current_yaw);
}


/*
=====================
idAI::FacingIdeal
=====================
*/
bool DnAI::FacingIdeal()
{
	float diff;

	if (!turnRate)
	{
		return true;
	}

	diff = idMath::AngleNormalize180(current_yaw - ideal_yaw);
	if (idMath::Fabs(diff) < 0.01f)
	{
		// force it to be exact
		current_yaw = ideal_yaw;
		return true;
	}

	return false;
}

/*
=====================
idAI::TurnToward
=====================
*/
bool DnAI::TurnToward(float yaw)
{
	ideal_yaw = idMath::AngleNormalize180(yaw);
	bool result = FacingIdeal();
	return result;
}

/*
=====================
DnAI::TurnToward
=====================
*/
bool DnAI::TurnToward(const idVec3& pos)
{
	idVec3 dir;
	idVec3 local_dir;
	float lengthSqr;

	dir = physicsObj.GetOrigin() - pos;
	physicsObj.GetGravityAxis().ProjectVector(dir, local_dir);
	local_dir.z = 0.0f;
	ideal_yaw = local_dir.ToYaw();

	return true;
}

/*
===============
DnAI::FindNewTarget
===============
*/
idPlayer* DnAI::FindNewTarget()
{
	idPlayer* localPlayer = gameLocal.GetLocalPlayer();

	if (CanSee(localPlayer, true))
	{
		return localPlayer;
	}

	return nullptr;
}

/*
=====================
DnAI::Turn
=====================
*/
void DnAI::Turn()
{
	float diff;
	float diff2;
	float turnAmount;

	if (!turnRate)
	{
		return;
	}


	diff = idMath::AngleNormalize180(ideal_yaw - current_yaw);
	turnVel += AI_TURN_SCALE * diff * MS2SEC(gameLocal.time - gameLocal.previousTime);
	if (turnVel > turnRate)
	{
		turnVel = turnRate;
	}
	else if (turnVel < -turnRate)
	{
		turnVel = -turnRate;
	}
	turnAmount = turnVel * MS2SEC(gameLocal.time - gameLocal.previousTime);
	if ((diff >= 0.0f) && (turnAmount >= diff))
	{
		turnVel = diff / MS2SEC(gameLocal.time - gameLocal.previousTime);
		turnAmount = diff;
	}
	else if ((diff <= 0.0f) && (turnAmount <= diff))
	{
		turnVel = diff / MS2SEC(gameLocal.time - gameLocal.previousTime);
		turnAmount = diff;
	}
	current_yaw += turnAmount;
	current_yaw = idMath::AngleNormalize180(current_yaw);
	diff2 = idMath::AngleNormalize180(ideal_yaw - current_yaw);
	if (idMath::Fabs(diff2) < 0.1f)
	{
		current_yaw = ideal_yaw;
	}

	viewAxis = idAngles(0, current_yaw, 0).ToMat3();
}


/*
=====================
DnAI::SlideMove
=====================
*/
void DnAI::SlideMove()
{
	idVec3				goalPos;
	idVec3				delta;
	idVec3				goalDelta;
	float				goalDist;
	monsterMoveResult_t	moveResult;
	idVec3				newDest;

	idVec3 oldorigin = physicsObj.GetOrigin();
	idMat3 oldaxis = viewAxis;

	AI_BLOCKED = false;

	goalPos = GetOrigin();

	if (move.moveCommand < NUM_NONMOVING_COMMANDS)
	{
		move.lastMoveOrigin.Zero();
		move.lastMoveTime = gameLocal.time;
	}

	move.obstacle = NULL;
	if ((move.moveCommand == MOVE_FACE_ENEMY) && target)
	{
		TurnToward(targetLastSeenLocation);
		goalPos = move.moveDest;
	}
	else if ((move.moveCommand == MOVE_FACE_ENTITY) && move.goalEntity.GetEntity())
	{
		TurnToward(move.goalEntity.GetEntity()->GetPhysics()->GetOrigin());
		goalPos = move.moveDest;
	}

	if (move.moveCommand == MOVE_SLIDE_TO_POSITION)
	{
		if (gameLocal.time < move.startTime + move.duration)
		{
			goalPos = move.moveDest - move.moveDir * MS2SEC(move.startTime + move.duration - gameLocal.time);
		}
		else
		{
			goalPos = move.moveDest;
			StopMove(MOVE_STATUS_DONE);
		}
	}

	if (move.moveCommand == MOVE_TO_POSITION)
	{
		goalDelta = move.moveDest - oldorigin;
		goalDist = goalDelta.LengthFast();
		if (goalDist < delta.LengthFast())
		{
			delta = goalDelta;
		}
	}

	idVec3 vel = physicsObj.GetLinearVelocity();
	float z = vel.z;
	idVec3  predictedPos = oldorigin + vel * AI_SEEK_PREDICTION;

	// seek the goal position
	//goalDelta = goalPos - predictedPos;
	vel -= vel * AI_FLY_DAMPENING * MS2SEC(gameLocal.time - gameLocal.previousTime);
	vel += goalDelta * MS2SEC(gameLocal.time - gameLocal.previousTime);

	float fly_speed = 100;

	// cap our speed
	vel = vel.Truncate(fly_speed);
	vel.z = z;
	physicsObj.SetLinearVelocity(vel);
	physicsObj.UseVelocityMove(true);
	RunPhysics();

	if ((move.moveCommand == MOVE_FACE_ENEMY) && target)
	{
		TurnToward(targetLastSeenLocation);
	}
	else if ((move.moveCommand == MOVE_FACE_ENTITY) && move.goalEntity.GetEntity())
	{
		TurnToward(move.goalEntity.GetEntity()->GetPhysics()->GetOrigin());
	}
	else if (move.moveCommand != MOVE_NONE)
	{
		if (vel.ToVec2().LengthSqr() > 0.1f)
		{
			TurnToward(vel.ToYaw());
		}
	}
	Turn();

	if (ai_debugMove.GetBool())
	{
		gameRenderWorld->DebugLine(colorCyan, oldorigin, physicsObj.GetOrigin(), 5000);
	}

	moveResult = physicsObj.GetMoveResult();

	AI_ONGROUND = physicsObj.OnGround();
}


/*
=====================
DnAI::StopMove
=====================
*/
void DnAI::StopMove(moveStatus_t status)
{
	//AI_MOVE_DONE = true;
	//AI_FORWARD = false;
	move.moveCommand = MOVE_NONE;
	move.moveStatus = status;
	move.toAreaNum = 0;
	move.goalEntity = NULL;
	move.moveDest = physicsObj.GetOrigin();
	//AI_DEST_UNREACHABLE = false;
	//AI_OBSTACLE_IN_PATH = false;
	AI_BLOCKED = false;
	move.startTime = gameLocal.time;
	move.duration = 0;
	move.range = 0.0f;
	move.speed = 0.0f;
	move.anim = 0;
	move.moveDir.Zero();
	move.lastMoveOrigin.Zero();
	move.lastMoveTime = gameLocal.time;
}


/*
===============
DnAI::SetAnimation
===============
*/
void DnAI::SetAnimation(const char* name, bool loop)
{
	int				animNum;
	int anim;
	const idAnim* newanim;

	animNum = animator.GetAnim(name);

	if (currentAnimation == name)
		return;

	if (!animNum) {
		gameLocal.Printf("Animation '%s' not found.\n", name);
		return;
	}

	anim = animNum;
	//starttime = gameLocal.time;
	//animtime = animator.AnimLength(anim);
	//headAnim = 0;

	currentAnimation = name;

	if (loop)
	{
		animator.CycleAnim(ANIMCHANNEL_ALL, anim, gameLocal.time, 0);
	}
	else
	{
		animator.PlayAnim(ANIMCHANNEL_ALL, anim, gameLocal.time, 0);
	}
	animator.RemoveOriginOffset(true);
}

bool DnAI::CurrentlyPlayingSound()
{
	if (refSound.referenceSound == nullptr)
		return false;

	return refSound.referenceSound->CurrentlyPlaying();
}


/*
=====================
DnAI::ReachedPos
=====================
*/
bool DnAI::ReachedPos(const idVec3& pos, const moveCommand_t moveCommand) const
{
	if (move.moveType == MOVETYPE_SLIDE)
	{
		idBounds bnds(idVec3(-4, -4.0f, -8.0f), idVec3(4.0f, 4.0f, 64.0f));
		bnds.TranslateSelf(physicsObj.GetOrigin());
		if (bnds.ContainsPoint(pos))
		{
			return true;
		}
	}
	else
	{
		if ((moveCommand == MOVE_TO_ENEMY) || (moveCommand == MOVE_TO_ENTITY))
		{
			if (physicsObj.GetAbsBounds().IntersectsBounds(idBounds(pos).Expand(8.0f)))
			{
				return true;
			}
		}
		else
		{
			idBounds bnds(idVec3(-16.0, -16.0f, -8.0f), idVec3(16.0, 16.0f, 64.0f));
			bnds.TranslateSelf(physicsObj.GetOrigin());
			if (bnds.ContainsPoint(pos))
			{
				return true;
			}
		}
	}
	return false;
}


/*
=====================
DnAI::MoveToPosition
=====================
*/
bool DnAI::MoveToPosition(const idVec3& pos)
{
	idVec3		org;
	int			areaNum;

	if (ReachedPos(pos, move.moveCommand))
	{
		StopMove(MOVE_STATUS_DONE);
		return true;
	}

	move.toAreaNum = 0;

	move.moveDest = pos;
	move.goalEntity = NULL;
	move.moveCommand = MOVE_TO_POSITION;
	move.moveStatus = MOVE_STATUS_MOVING;
	move.startTime = gameLocal.time;
	move.speed = 100;


	return true;
}

void DnAI::UpdatePathToPosition(idVec3 pos)
{
	if (pathWaypoints.Num() > 0)
	{
		float len = (pos - pathWaypoints[pathWaypoints.Num() - 1]).Length();
		if (len < 150)
		{
			len = (GetOrigin() - pathWaypoints[waypointId]).Length();
			if (len < 150)
			{
				waypointId++;

				if (waypointId >= pathWaypoints.Num())
					waypointId = pathWaypoints.Num() - 1;
			}

			MoveToPosition(pathWaypoints[waypointId]);

			return;
		}
	}

	waypointId = 0;
	pathWaypoints.Clear();
	if (!gameLocal.GetNavigation()->GetPathBetweenPoints(GetOrigin(), pos, pathWaypoints))
	{
		return;
	}

	MoveToPosition(pathWaypoints[0]);
}

bool DnAI::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location)
{
	return true;
}

void DnAI::Killed(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location)
{
	if (startedDeath)
	{
		return;
	}

	startedDeath = true;
	SetState("state_BeginDeath");
}

void DnAI::Think(void)
{
	idActor::Think();

	// Update the last time we have seen our target.
	if (target)
	{
		if (CanSee(target, false))
		{
			targetLastSeenLocation = target->GetOrigin();
			isTargetVisible = true;
		}
		else
		{
			isTargetVisible = false;
		}
	}

	stateThread.Execute();

	if (health > 0)
	{
		if (move.moveCommand != MOVE_NONE)
		{
			SlideMove();
		}
		else
		{
			current_yaw = ideal_yaw;
			viewAxis = idAngles(0, ideal_yaw, 0).ToMat3();
			SetAxis(viewAxis);
		}
	}
}