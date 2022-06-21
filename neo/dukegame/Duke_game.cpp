// Duke_game.cpp
//

#include "../game/Game_local.h"

/*
=====================
dnGameLocal::DrawPortalSky
=====================
*/
void dnGameLocal::DrawPortalSky(renderView_t& hackedView)
{
	if (gamePortalSkyWorld != nullptr && g_enablePortalSky.GetBool()) {
		renderView_t	portalView = hackedView;
		portalView.vieworg = gamePortalSkyWorld->GetPortalSkyCameraPosition();

		// setup global fixup projection vars
		if (1) {
			int vidWidth, vidHeight;
			idVec2 shiftScale;

			renderSystem->GetGLSettings(vidWidth, vidHeight);

			float pot;
			int	 w = vidWidth;
			pot = MakePowerOfTwo(w);
			shiftScale.x = (float)w / pot;

			int	 h = vidHeight;
			pot = MakePowerOfTwo(h);
			shiftScale.y = (float)h / pot;

			hackedView.shaderParms[4] = shiftScale.x;
			hackedView.shaderParms[5] = shiftScale.y;
		}

		gamePortalSkyWorld->RenderScene(&portalView);
		renderSystem->CaptureRenderToImage("_currentRender");

		hackedView.forceUpdate = true;				// FIX: for smoke particles not drawing when portalSky present
	}
}

/*
=====================
dnGameLocal::Draw
=====================
*/
bool dnGameLocal::Draw(int clientNum) {
	idPlayer* player = GetLocalPlayer();
	const renderView_t* view = player->GetRenderView();

	idUserInterface* hud = player->hud;

	// place the sound origin for the player
	gameSoundWorld->PlaceListener(view->vieworg, view->viewaxis, player->entityNumber + 1, gameLocal.slow.time, hud ? hud->State().GetString("location") : "Undefined");

	// hack the shake in at the very last moment, so it can't cause any consistency problems
	renderView_t	hackedView = *view;
	hackedView.viewaxis = hackedView.viewaxis; // *ShakeAxis();

	// Draw the portal sky.
	DrawPortalSky(hackedView);

	// do the first render
	gameRenderWorld->RenderScene(&hackedView);

	player->DrawHUD(hud);

	return true;
}