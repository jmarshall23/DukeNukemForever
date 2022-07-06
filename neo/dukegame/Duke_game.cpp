// Duke_game.cpp
//

#include "../game/Game_local.h"

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
idEntity* dnGameLocal::HitScan(const idVec3& origin, const idVec3& dir, const idVec3& origFxOrigin, idEntity* owner, bool noFX, float damageScale, idEntity* additionalIgnore, int	areas[2], bool spawnDebris, float range, int push)
{
	idVec3		start;
	idVec3		end;
	int			contents;
	trace_t		tr;

	idEntity* ent = nullptr;

	// Calculate the end point of the trace
	start = origin;
	end = start + (dir.ToMat3() * idVec3(idMath::ClampFloat(0, 2048, range), 0, 0));

	//gameRenderWorld->DebugLine(colorWhite, start, end, 1000);

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
#if 1
	if (!ent->fl.takedamage && spawnDebris)
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
#endif
	return ent;
}
