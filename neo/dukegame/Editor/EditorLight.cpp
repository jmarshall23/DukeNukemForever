// EditorLight.cpp
//

#include "../../game/Game_local.h"

/*
===================
dnEditorLight::dnEditorLight
===================
*/
dnEditorLight::dnEditorLight(idRenderWorld* editorRenderWorld) {
	idDict args;
	gameLocal.ParseSpawnArgsToRenderLight(&args, &renderLightParams);
	renderLightHandle = editorRenderWorld->AddLightDef(&renderLightParams);

	this->editorRenderWorld = editorRenderWorld;
}

/*
===================
dnEditorLight::~dnEditorLight
===================
*/
dnEditorLight::~dnEditorLight() {
	if (renderLightHandle != -1)
	{
		editorRenderWorld->FreeLightDef(renderLightHandle);
		renderLightHandle = -1;
	}
}

/*
===============
dnEditorLight::Render
===============
*/
void dnEditorLight::Render(idDict& spawnArgs, bool isSelected) {
	gameLocal.ParseSpawnArgsToRenderLight(&spawnArgs, &renderLightParams);

	if (isSelected)
	{
		DrawSelectedGizmo(renderLightParams.origin);
	}

	editorRenderWorld->UpdateLightDef(renderLightHandle, &renderLightParams);
}