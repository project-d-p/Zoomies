#pragma once

// Compile mode
// ------------------
// EDITOR_MODE | LAN_MODE
//     0       |    0     : Online
//     0       |    1     : LAN
//     1       |    0     : Editor

#ifndef EDITOR_MODE
	#define EDITOR_MODE 0
#endif

#ifndef LAN_MODE
	#define LAN_MODE 0
#endif

namespace Zoomies	
{
	constexpr int MAX_MONSTERS = 0;
	constexpr int MAX_PLAYERS = 1;
	constexpr float GAME_TIME = 4.f;
	constexpr float JUDGE_TIME = 10000.f;
}