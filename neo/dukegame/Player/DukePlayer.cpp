// DukePlayer.cpp
//

#include "../../game/game_local.h"

CLASS_DECLARATION(idPlayer, DukePlayer)
END_CLASS

void DukePlayer::UpdateHudStats(idUserInterface* hud)
{
	hud->SetStateInt("player_ego", health);
}