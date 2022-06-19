// Duke_game.h
//

//
// dnGameLocal
//
class dnGameLocal : public idGameLocal {
public:
	virtual bool			Draw(int clientNum);

	virtual DukePlayer* GetLocalDukePlayer() { return (DukePlayer*)GetLocalPlayer(); }
private:
	void					DrawPortalSky(renderView_t &hackedView);
};