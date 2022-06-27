// DukePlayer.cpp
//

#include "../../game/game_local.h"

const idEventDef EV_Player_DukeTalk("startDukeTalk", "s");

CLASS_DECLARATION(idPlayer, DukePlayer)
	EVENT(EV_Player_DukeTalk, DukePlayer::Event_DukeTalk)
END_CLASS

void DukePlayer::UpdateHudStats(idUserInterface* hud)
{
	hud->SetStateInt("player_ego", health);
	UpdateHudAmmo(hud);
}

void DukePlayer::Event_DukeTalk(const char* soundName)
{
	gameSoundWorld->PlayShaderDirectly(soundName, SCHANNEL_DUKETALK);
}

void DukePlayer::SetStartingInventory(void)
{
	inventory.ammo[DN_WEAPON_PISTOL] = 48;

	Give("weapon", "weapon_mightyfoot");
	Give("weapon", "weapon_pistol");
}