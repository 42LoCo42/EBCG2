#include "SaveState.hpp"
#include "BoardControl.hpp"

SaveState::SaveState(int x, int y) {
	setupBoard(board, x, y);
	score = 0;
	currentNum = nextNum(6);
}
