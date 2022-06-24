// LightComponent.cpp
//

#include "../../game/Game_local.h"

DnLightComponent::DnLightComponent()
{
	parentEntity = nullptr;
	renderLightHandle = -1;
	bindJoint = (jointHandle_t)-1;
	memset(&renderLightParams, 0, sizeof(renderLightParams));
}

DnLightComponent::~DnLightComponent()
{
	if (renderLightHandle != -1)
	{
		gameRenderWorld->FreeLightDef(renderLightHandle);
		renderLightHandle = -1;
	}
}

void DnLightComponent::Init(idEntity* parent, idVec3 radius, idVec3 color, bool noShadows)
{
	idDict args;
	args.SetVector("light_radius", radius);
	args.SetVector("_color", color);
	args.SetBool("noshadows", noShadows);

	gameEdit->ParseSpawnArgsToRenderLight(&args, &renderLightParams);
	renderLightHandle = gameRenderWorld->AddLightDef(&renderLightParams);
	parentEntity = parent;
}

void DnLightComponent::BindToJoint(const char* jointName)
{
	idAnimatedEntity* animatedEntity = parentEntity->Cast<idAnimatedEntity>();
	if (animatedEntity == nullptr)
	{
		gameLocal.Error("You can't bind to a joint without the parent deriving from idAnimatedEntity!");
	}

	bindJoint = animatedEntity->GetAnimator()->GetJointHandle(jointName);
	if (bindJoint == -1)
	{
		gameLocal.Error("Failed to bind to joint %s", jointName);
	}
}

void DnLightComponent::Think()
{
	if (bindJoint != -1)
	{
		idAnimatedEntity* animatedEntity = parentEntity->Cast<idAnimatedEntity>();
		if (animatedEntity == nullptr)
		{
			gameLocal.Error("You can't bind to a joint without the parent deriving from idAnimatedEntity!");
		}


		animatedEntity->GetJointWorldTransform(bindJoint, gameLocal.time, renderLightParams.origin, renderLightParams.axis);
	}
	else
	{
		renderLightParams.origin = parentEntity->GetOrigin();
		renderLightParams.axis = parentEntity->GetAxis();
	}

	gameRenderWorld->UpdateLightDef(renderLightHandle, &renderLightParams);
}