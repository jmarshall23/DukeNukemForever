// Duke_game.cpp
//

#include "../game/Game_local.h"

idCVar g_drawVisibleLights("g_drawVisibleLights", "0", CVAR_GAME | CVAR_CHEAT, "");

/*
===================
idGameLocal::IsParentalLockEnabled
===================
*/
bool dnGameLocal::IsParentalLockEnabled(void)
{
	return g_ParentalLock.GetBool();
}

/*
===================
idGameLocal::TracePoint
===================
*/
bool dnGameLocal::TracePoint(const idEntity* ent, trace_t& results, const idVec3& start, const idVec3& end, int contentMask, const idEntity* passEntity) {	
	return clip.TracePoint(results, start, end, contentMask, passEntity);
}

/*
=====================
dnGameLocal::HitScan
=====================
*/
idEntity* dnGameLocal::HitScan(const idVec3& origin, const idVec3& dir, const idVec3& origFxOrigin, idEntity* owner, bool noFX, float damageScale, idEntity* additionalIgnore, int	areas[2], float range, int push)
{
	idVec3		start;
	idVec3		end;
	int			contents;
	trace_t		tr;

	idEntity* ent = nullptr;

	// Calculate the end point of the trace
	start = origin;
	end = start + (dir.ToMat3() * idVec3(idMath::ClampFloat(0, 2048, range), 0, 0));
	contents = MASK_SHOT_RENDERMODEL | CONTENTS_WATER | CONTENTS_PROJECTILE;

	TracePoint(owner, tr, start, end, contents, additionalIgnore);

	if (tr.fraction >= 1.0f || (tr.c.material && tr.c.material->GetSurfaceFlags() & SURF_NOIMPACT)) {
		return nullptr;
	}

	ent = entities[tr.c.entityNum];

	if (!gameLocal.isClient) {
		if (ent->fl.takedamage)
		{
			ent->Damage(owner, owner, dir, "damage_generic", damageScale, CLIPMODEL_ID_TO_JOINT_HANDLE(tr.c.id));
		}
	}

	if (!ent->fl.takedamage)
	{
		// spawn debris entities
		int fxdebris = 12; // spawnArgs.GetInt("debris_count");
		if (fxdebris) {
			const idDict* debris = gameLocal.FindEntityDefDict("debris_brass", false);
			if (debris) {
				int amount = gameLocal.random.RandomInt(fxdebris);
				for (int i = 0; i < amount; i++) {
					idEntity* ent;
					idVec3 dir;
					dir.x = gameLocal.random.CRandomFloat() * 4.0f;
					dir.y = gameLocal.random.CRandomFloat() * 4.0f;
					dir.z = gameLocal.random.RandomFloat() * 8.0f;
					dir.Normalize();

					gameLocal.SpawnEntityDef(*debris, &ent, false);
					if (!ent || !ent->IsType(idDebris::Type)) {
						gameLocal.Error("'projectile_debris' is not an idDebris");
					}

					idDebris* debris = static_cast<idDebris*>(ent);
					debris->Create(owner, tr.c.point, dir.ToMat3());
					debris->Launch(tr.c.material);
				}
			}
			debris = gameLocal.FindEntityDefDict("projectile_shrapnel", false);
			if (debris) {
				int amount = gameLocal.random.RandomInt(fxdebris);
				for (int i = 0; i < amount; i++) {
					idEntity* ent;
					idVec3 dir;
					dir.x = gameLocal.random.CRandomFloat() * 8.0f;
					dir.y = gameLocal.random.CRandomFloat() * 8.0f;
					dir.z = gameLocal.random.RandomFloat() * 8.0f + 8.0f;
					dir.Normalize();

					gameLocal.SpawnEntityDef(*debris, &ent, false);
					if (!ent || !ent->IsType(idDebris::Type)) {
						gameLocal.Error("'projectile_shrapnel' is not an idDebris");
					}

					idDebris* debris = static_cast<idDebris*>(ent);
					debris->Create(owner, tr.c.point, dir.ToMat3());
					debris->Launch(tr.c.material);
				}
			}
		}
	}

	return ent;
}

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

	// Debug command to draw visible lights
	if (g_drawVisibleLights.GetBool())
	{
		for (int i = 0; i < gameRenderWorld->GetNumRenderLights(); i++)
		{
			const renderLight_t* light = gameRenderWorld->GetRenderLight(i);

			if (!gameRenderWorld->IsRenderLightVisible(i))
			{
				continue;
			}

			idBox box = idBox(light->origin, idVec3(30, 30, 30), light->axis);
			gameRenderWorld->DebugBox(colorGreen, box);
		}
	}

	// do the first render
	gameRenderWorld->RenderScene(&hackedView);

	player->DrawHUD(hud);

	return true;
}