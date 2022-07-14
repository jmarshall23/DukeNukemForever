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