// Duke_game.h
//

#include "GameSys/DukeCvar.h"
#include "Renderer/DnFullscreenRenderTarget.h"

//
// DnRenderPlatform
//
struct DnRenderPlatform
{
	DnFullscreenRenderTarget* frontEndPassRenderTarget;
	DnFullscreenRenderTarget* frontEndPassRenderTargetResolved;

	const idMaterial* upscaleFrontEndResolveMaterial;
};

//
// dnGameLocal
//
class dnGameLocal : public idGameLocal {
public:
	virtual void			InitGameRender();

	virtual bool			Draw(int clientNum);

	virtual DukePlayer* GetLocalDukePlayer() { return (DukePlayer*)GetLocalPlayer(); }

	idEntity*				HitScan(const idVec3& origOrigin, const idVec3& origDir, const idVec3& origFxOrigin, idEntity* owner, bool noFX, float damageScale, idEntity* additionalIgnore, int	areas[2], float range = 2048, int push = 5000);
	bool					TracePoint(const idEntity* ent, trace_t& results, const idVec3& start, const idVec3& end, int contentMask, const idEntity* passEntity);

	bool					IsParentalLockEnabled(void);
private:
	void					DrawPortalSky(renderView_t &hackedView);

	DnRenderPlatform		renderPlatform;
};