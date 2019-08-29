#pragma once
#include <string>

#include "defs.hpp"
const std::string TEMPLATE = "{\"score\": 0,\"maxScore\": 0,\"number\": 0,\"board\": []}";

struct SaveState {
	board_t board;
	int score;
	int maxScore;
	int currentNum;

	SaveState(int x = 5, int y = 7);
	// SaveState(const std::string& data); /** deserialization */
	void toString(std::string& result);  /** serialization */
};
