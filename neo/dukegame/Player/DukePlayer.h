// DukePlayer.h
//

enum dnWeapons {
	DN_WEAPON_FEET = 0,
	DN_WEAPON_PISTOL,
	DN_WEAPON_SHOTGUN
};

//
// DnPlayer
//
class DukePlayer : public idPlayer {
public:
	CLASS_PROTOTYPE(DukePlayer);

	DukePlayer();

	virtual void			UpdateHudStats(idUserInterface* hud);
	virtual void			SetStartingInventory(void);

	void					Event_DukeTalk(const char* soundName);

	virtual void			BobCycle(const idVec3& pushVelocity) override;
private:
	idVec3					ApplyLandDeflect(const idVec3& pos, float scale);

	float					bob;
	float					lastAppliedBobCycle;
};