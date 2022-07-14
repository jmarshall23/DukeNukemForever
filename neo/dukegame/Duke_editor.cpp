// Duke_editor.cpp
//

#include "../game/Game_local.h"

/*
======================
idGameEdit::AllocEditorEntity
======================
*/
dnEditorEntity* idGameEdit::AllocEditorEntity(idRenderWorld* editorRenderWorld, dnEditorEntityType_t editorEntityType) {
	switch (editorEntityType)
	{
		case EDITOR_ENTITY_GENERIC:
			return new dnEditorEntity(editorRenderWorld);

		case EDITOR_ENTITY_LIGHT:
			return new dnEditorLight(editorRenderWorld);

		case EDITOR_ENTITY_MODEL:
			return new dnEditorModel(editorRenderWorld);
	}
	return nullptr;
}

/*
======================
idGameEdit::FreeEditorEntity
======================
*/
void idGameEdit::FreeEditorEntity(dnEditorEntity* entity) {
	delete entity;
}

/*
======================
idGameEdit::DrawSelectedGizmo
======================
*/
void dnEditorEntity::DrawSelectedGizmo(idVec3 origin) {
	editorRenderWorld->DebugArrow(colorRed, origin, origin + idVec3(0, 0, 75), 25);
	editorRenderWorld->DebugArrow(colorGreen, origin, origin + idVec3(0, 75, 0), 25);
	editorRenderWorld->DebugArrow(colorBlue, origin, origin + idVec3(75, 0, 0), 25);
}

/*
======================
idGameEdit::Render
======================
*/
void dnEditorEntity::Render(idDict& spawnArgs, bool isSelected, const renderView_t& renderView) {
	idVec3 maxs = spawnArgs.GetVector("editor_maxs");
	idVec3 mins = spawnArgs.GetVector("editor_mins");

	idVec3 origin = spawnArgs.GetVector("origin");

	idStr name = spawnArgs.GetString("name");
	if (name == "")
	{
		name = spawnArgs.GetString("classname");
	}

	editorRenderWorld->DebugBounds(colorWhite, idBounds(mins, maxs), origin, 0, true);
	editorRenderWorld->DrawTextA(name, origin + idVec3(0, 0, maxs.z), 0.5f, colorWhite, renderView.viewaxis, 1, 0, true);
}