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

	virtual void			UpdateHudStats(idUserInterface* hud);
	virtual void			SetStartingInventory(void);
};