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

	void					Spawn(void);

	virtual void			UpdateHudStats(idUserInterface* hud);
	virtual void			SetStartingInventory(void);

	void					Event_DukeTalk(const char* soundName);

	void					Event_PlayDukeJumpSound(void);
	void					Event_PlayRandomDukeTaunt(void);

	virtual void			BobCycle(const idVec3& pushVelocity) override;
private:
	idVec3					ApplyLandDeflect(const idVec3& pos, float scale);

	const idSoundShader*	dukeTauntShader;

	idList<const idSoundShader*> dukeJumpSounds;

	bool					firstSwearTaunt;

	float					bob;
	float					lastAppliedBobCycle;
};