// Duke_game.h
//

//
// dnGameLocal
//
class dnGameLocal : public idGameLocal {
public:
	virtual bool			Draw(int clientNum);
private:
	void					DrawPortalSky(renderView_t &hackedView);
};