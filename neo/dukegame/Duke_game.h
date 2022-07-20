// Duke_game.h
//

class rvClientEntity;

// RAVEN BEGIN
// bdube: dummy entity for client side physics
#define ENTITYNUM_CLIENT		(MAX_GENTITIES-3)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-3)
// RAVEN END

// RAVEN BEGIN
// bdube: client entities
#define	CENTITYNUM_BITS			12
#define	MAX_CENTITIES			(1<<CENTITYNUM_BITS)
// shouchard:  for ban lists and because I hate magic numbers
#define CLIENT_GUID_LENGTH		12
// RAVEN END

#include "GameSys/DukeCvar.h"
#include "Renderer/DnFullscreenRenderTarget.h"
#include "Duke_editor.h"

//
// DnRenderPlatform
//
struct DnRenderPlatform
{
	DnFullscreenRenderTarget* frontEndPassRenderTarget;
	DnFullscreenRenderTarget* frontEndPassRenderTargetResolved;
	DnFullscreenRenderTarget* ssaoRenderTarget;

	const idMaterial* upscaleFrontEndResolveMaterial;
	const idMaterial* ssaoMaterial;
	const idMaterial* ssaoBlurMaterial;
	const idMaterial* bloomMaterial;
	const idMaterial* blackMaterial;
};

//
// dnGameLocal
//
class dnGameLocal : public idGameLocal {
public:
	virtual void			Init(void) override;

	virtual bool			Draw(int clientNum);
	virtual void			StartMainMenu(bool playIntro);

	virtual void			HandleInGameCommands(const char* menuCommand);
	virtual void			HandleMainMenuCommands(const char* menuCommand);
	virtual const char*		HandleGuiCommands(const char* menuCommand);

	void					RegisterClientEntity(rvClientEntity* cent);
	void					UnregisterClientEntity(rvClientEntity* cent);

	bool					SpawnClientEntityDef(const idDict& args, rvClientEntity** cent, bool setDefaults, const char* spawn);

	virtual idUserInterface* GetMainMenuUI(void);

	virtual DukePlayer* GetLocalDukePlayer() { return (DukePlayer*)GetLocalPlayer(); }

	idEntity*				HitScan(const idVec3& origOrigin, const idVec3& origDir, const idVec3& origFxOrigin, idEntity* owner, bool noFX, float damageScale, idEntity* additionalIgnore, int	areas[2], bool spawnDebris, float range = 2048, int push = 5000);
	bool					TracePoint(const idEntity* ent, trace_t& results, const idVec3& start, const idVec3& end, int contentMask, const idEntity* passEntity);

	bool					IsParentalLockEnabled(void);
private:
	void					DrawPortalSky(renderView_t &hackedView);

public:
	DnRenderPlatform		renderPlatform;

	idUserInterface*		guiMainMenu;

// RAVEN BEGIN
// bdube: client entities
	rvClientEntity* clientEntities[MAX_CENTITIES];	// index to client entities
	int							clientSpawnIds[MAX_CENTITIES];	// for use in idClientEntityPtr
	idLinkList<rvClientEntity>	clientSpawnedEntities;			// all client side entities
	int							num_clientEntities;				// current number of client entities
	int							firstFreeClientIndex;			// first free index in the client entities array
	int							clientSpawnCount;
	int							entityRegisterTime;
// RAVEN END

private:
	const static int		INITIAL_SPAWN_COUNT = 1;

	void					InitGuis();
	void					InitGameRender();	
public:
	static void				ClientEntityJob_t(void* params);

	idSysMutex				clientGamePhysicsMutex;

	idParallelJobList* clientPhysicsJob;
	idList<rvClientEntity*>		clientEntityThreadWork;
};