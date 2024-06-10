#pragma once

#include <string>
// This Class Is For Local Test Only
class PlayerName
{
private:
	static std::string player_one;
public:
	static std::string& GetNamePlayerOne();
	// static std::string& GetNamePlayerTwo();
};
