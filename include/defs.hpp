#pragma once

#include <vector>

const int STAIR_PRIO_AFTER_MERGE = 3;
const int DEFAULT_PRIO = 1;

struct cell {
	int num = 0, prio = 0;
	int futureNum = 0, futurePrio = 0;

	cell(int _num = 0, int _prio = 0) : num(_num), prio(_prio) {};
};
typedef std::vector<cell> column_t;
typedef std::vector<column_t> board_t;
