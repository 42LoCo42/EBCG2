#pragma once

#include <vector>

const int STAIR_PRIO_AFTER_MERGE = 3;
const int DEFAULT_PRIO = 1;

struct cell {
	int num = 0, prio = 0;
	int futureNum = 0, futurePrio = 0;
};
typedef std::vector<cell> column_t;
typedef std::vector<column_t> board_t;

enum GameState {
	mainMenu, openMenu, settingsMenu,
	playing, pauseMenu, saveMenu,
	shop, timeline,
	quit
};
