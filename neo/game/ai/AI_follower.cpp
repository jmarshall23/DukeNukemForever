// AI_follower.cpp
//

#pragma hdrstop
#include "precompiled.h"

#include "../Game_local.h"

CLASS_DECLARATION( idAI, rvmAI_Follower )
END_CLASS

#define FOLLOW_MAXDIST	120
#define FOLLOW_MINDIST	80
#define FOLLOW_RUNDIST	180

/*
================
rvmAI_Follower::Init
================
*/
void rvmAI_Follower::Init( void )
{
	inCustomAnim = false;
	AI_RUN = false;

	//float mod;
	//mod = getIntKey("head_look");
	//setBoneMod(mod);

	leader = NULL;

	Event_SetTalkState( TALK_OK );

	Event_SetKey( "conversationNext", "" );

	Event_SetKey( "wander", "1" );

	Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 0 );
	Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 0 );
	stateThread.SetState( "state_idle" );
}

/*
================
rvmAI_Follower::state_idle
================
*/
stateResult_t rvmAI_Follower::state_idle( stateParms_t* parms )
{
	Event_StopMove();
	Event_SetTalkTarget( NULL );
	stateThread.SetState( "state_idle_frame" );
	return SRESULT_DONE;
}

/*
================
rvmAI_Follower::state_idle_frame
================
*/
stateResult_t rvmAI_Follower::state_idle_frame( stateParms_t* parms )
{
	if( AI_TALK )
	{
		PlayCustomAnim( "melee_attack1", 0, 0 );
		stateThread.SetState( "state_talk_anim" );
		return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}


/*
================
rvmAI_Follower::state_idle_frame
================
*/
stateResult_t rvmAI_Follower::state_talk_anim( stateParms_t* parms )
{
	if( AnimDone( ANIMCHANNEL_TORSO, 0 ) )
	{
		stateThread.SetState( "state_follow" );
		Event_AnimState( ANIMCHANNEL_TORSO, "Torso_Idle", 0 );
		Event_AnimState( ANIMCHANNEL_LEGS, "Legs_Idle", 0 );
		return SRESULT_DONE;
	}
	return SRESULT_WAIT;
}

/*
================
rvmAI_Follower::state_follow
================
*/
stateResult_t rvmAI_Follower::state_follow( stateParms_t* parms )
{
	leader = talkTarget.GetEntity();
	if( !leader )
	{
		leader = gameLocal.GetLocalPlayer();
	}

	Event_SetTalkTarget( NULL );

	stateThread.SetState( "state_follow_frame" );
	return SRESULT_DONE;
}

/*
================
rvmAI_Follower::state_follow_frame
================
*/
stateResult_t rvmAI_Follower::state_follow_frame( stateParms_t* parms )
{
	Event_LookAtEntity( leader, 0.1f );

	if( AI_TALK )
	{
		stateThread.SetState( "state_idle" );
		return SRESULT_DONE;
	}

	if( DistanceTo( leader ) > FOLLOW_MAXDIST )
	{
		leader = talkTarget.GetEntity();
		if( !leader )
		{
			leader = gameLocal.GetLocalPlayer();
		}
		Event_SetTalkTarget( NULL );
		AI_RUN = false;
		Event_MoveToEntity( leader );
		stateThread.SetState( "state_get_closer" );
	}
	else
	{
		Event_FaceEntity( leader );
	}
	return SRESULT_WAIT;
}

/*
================
rvmAI_Follower::state_get_closer
================
*/
stateResult_t rvmAI_Follower::state_get_closer( stateParms_t* parms )
{
	bool switchState = !( !AI_DEST_UNREACHABLE && !AI_MOVE_DONE && ( DistanceTo( leader ) > FOLLOW_MINDIST ) );
	if( switchState )
	{
		stateThread.SetState( "state_follow_frame" );
		Event_StopMove();
		return SRESULT_DONE;
	}

	Event_LookAtEntity( leader, 0.1f );
	if( DistanceTo( leader ) > FOLLOW_RUNDIST )
	{
		AI_RUN = true;
	}
	if( DistanceTo( leader ) < FOLLOW_MAXDIST )
	{
		AI_RUN = false;
	}
	if( AI_TALK )
	{
		stateThread.SetState( "state_Idle" );
	}
	return SRESULT_WAIT;
}

/*
================
rvmAI_Follower::state_killed
================
*/
stateResult_t rvmAI_Follower::state_killed( stateParms_t* parms )
{
	Event_StopMove();
	return SRESULT_DONE;
}

