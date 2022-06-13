#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

/*
================================================

Torso Animations

================================================
*/

/*
===================
idAI::Torso_Idle
===================
*/
stateResult_t idAI::Torso_Idle(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_IdleAnim(ANIMCHANNEL_TORSO, "idle");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AI_PAIN)
		{
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 3);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
idAI::Torso_MeleeAttack
===================
*/
stateResult_t idAI::Torso_MeleeAttack(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_PlayAnim(ANIMCHANNEL_TORSO, "melee_attack");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 8))
		{
			Event_FinishAction("melee_attack");
			Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 8);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
idAI::Torso_RangeAttack
===================
*/
stateResult_t idAI::Torso_RangeAttack(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_DisablePain();
		Event_FaceEnemy();
		Event_PlayAnim(ANIMCHANNEL_TORSO, "turret_attack");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 8))
		{
			Event_AllowMovement(true);
			Event_FinishAction("range_attack");

			if (canSwitchToIdleFromRange)
			{
				Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 8);
			}
			return SRESULT_DONE;
		}
		else
		{
			Event_LookAtEnemy(1.0f);
		}

		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}

/*
===================
idAI::Torso_Death
===================
*/
stateResult_t idAI::Torso_Death(stateParms_t* parms) {
	Event_FinishAction("dead");
	return SRESULT_DONE;
}

/*
===================
idAI::Torso_Sight
===================
*/
stateResult_t idAI::Torso_Sight(stateParms_t* parms) {
	idStr animname;
	float blendFrames;

	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	animname = GetKey("on_activate");
	if (GetKey("on_activate_blend")) {
		blendFrames = GetIntKey("on_activate_blend");
	}
	else {
		blendFrames = 4;
	}

	switch (parms->stage) {
		case STAGE_INIT: 
			if (GetIntKey("walk_on_sight")) {
				Event_OverrideAnim(ANIMCHANNEL_LEGS);
			}

			if (HasAnim(ANIMCHANNEL_TORSO, animname, false))
			{
				Event_PlayAnim(ANIMCHANNEL_TORSO, animname);
				SRESULT_STAGE(STAGE_WAIT);
			}

			return SRESULT_DONE;
		case STAGE_WAIT:
			if (AnimDone(ANIMCHANNEL_TORSO, blendFrames)) {
				Event_FinishAction("sight");
				Event_AnimState(ANIMCHANNEL_TORSO, "Torso_Idle", blendFrames);
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
===================
idAI::Torso_Pain
===================
*/
stateResult_t idAI::Torso_Pain(stateParms_t* parms) {
	Event_PlayAnim(ANIMCHANNEL_TORSO, painAnim.Length() ? painAnim : "pain");
	PostAnimState(ANIMCHANNEL_TORSO, "Wait_TorsoAnim", 4);
	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Idle", 4);
	return SRESULT_DONE;
}

/*
================================================

Legs Animations

================================================
*/

/*
===================
idAI::Legs_Idle
===================
*/
stateResult_t idAI::Legs_Idle(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};
	
	switch (parms->stage) {
		case STAGE_INIT:
			Event_IdleAnim(ANIMCHANNEL_LEGS, "idle");
			parms->stage = STAGE_WAIT;
			return SRESULT_WAIT;

		case STAGE_WAIT:
			if (can_run && AI_RUN && AI_FORWARD)
			{
				Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Run", 8);
				return SRESULT_DONE;
			}
			if (AI_FORWARD)
			{
				Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Walk", 8);
			}
			return SRESULT_WAIT;		
	}

	return SRESULT_DONE;
}

/*
===================
idAI::Legs_Walk
===================
*/
stateResult_t idAI::Legs_Walk(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_PlayCycle(ANIMCHANNEL_LEGS, "walk");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (can_run && AI_RUN && AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Run", 8);
			return SRESULT_DONE;
		}
		if (!AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Idle", 8);
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}


/*
===================
idAI::Legs_Run
===================
*/
stateResult_t idAI::Legs_Run(stateParms_t* parms) {
	enum {
		STAGE_INIT = 0,
		STAGE_WAIT,
	};

	switch (parms->stage) {
	case STAGE_INIT:
		Event_PlayCycle(ANIMCHANNEL_LEGS, "run");
		parms->stage = STAGE_WAIT;
		return SRESULT_WAIT;

	case STAGE_WAIT:
		if (!AI_RUN && AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Walk", 8);
			return SRESULT_DONE;
		}
		if (!AI_FORWARD)
		{
			Event_AnimState(ANIMCHANNEL_LEGS, "Legs_Idle", 8);
		}
		return SRESULT_WAIT;
	}

	return SRESULT_DONE;
}