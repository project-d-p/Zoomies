#include "PlayerName.h"

std::string PlayerName::player_one = "DPCharacter_1";

std::string& PlayerName::GetNamePlayerOne()
{
	return player_one;
}