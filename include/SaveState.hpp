#pragma once

#include <string>

#include "defs.hpp"

struct SaveState {
	board_t board;
	int score;
	int currentNum;
	int currentCol;

	SaveState(int x = 5, int y = 7);
	// SaveState(std::string); /** deserialization */
	// std::string toString();  /** serialization */
};
