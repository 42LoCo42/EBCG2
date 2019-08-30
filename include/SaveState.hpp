#pragma once
#include <string>

#include "defs.hpp"
const std::string TEMPLATE = "{\"score\": 0,\"maxScore\": 0,\"currentNum\": 0}";

struct SaveState {
	board_t board;
	int score;
	int maxScore;
	int currentNum;

	void init(int x = 5, int y = 7);
	void init(const std::string& data); /** deserialization */
	std::string toString();  /** serialization */
};
