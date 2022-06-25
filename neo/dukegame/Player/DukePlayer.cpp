// DukePlayer.cpp
//

#include "../../game/game_local.h"

CLASS_DECLARATION(idPlayer, DukePlayer)
END_CLASS

void DukePlayer::UpdateHudStats(idUserInterface* hud)
{
	hud->SetStateInt("player_ego", health);
	UpdateHudAmmo(hud);
}

void DukePlayer::SetStartingInventory(void)
{
	inventory.ammo[DN_WEAPON_PISTOL] = 48;

	Give("weapon", "weapon_mightyfoot");
	Give("weapon", "weapon_pistol");
}