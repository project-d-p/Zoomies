#pragma once

// Compile mode
// ------------------
// EDITOR_MODE | LAN_MODE
//     0       |    0     : Online
//     0       |    1     : LAN
//     1       |    0     : Editor

#define EDITOR_MODE 1

#ifndef EDITOR_MODE
	#define EDITOR_MODE 0
#endif

#ifndef LAN_MODE
	#define LAN_MODE 0
#endif