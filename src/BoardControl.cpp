#include <stdlib.h> // for rand & srand
#include <time.h> // to generate a seed
#include <cmath> // for score calculation (pow)

#include "defs.hpp"
#include "distribute.hpp"
#include "BoardControl.hpp"
#include "SaveState.hpp"

using namespace std;

extern SaveState save;
bool boardChanged = false; // set if a step() changed the board -> redo step()
bool seeded = false;

int nextNum(int max) {
	if(!seeded) srand(time(NULL));

	return rand() % max + 1;
}

void setupBoard(board_t& board, int xLen, int yLen) {
	save.board = board_t(xLen);

	for(int x=0; x<xLen; ++x) {
		save.board[x] = column_t(yLen); // cells are created automagically
	}
}

void enlarge(const enlargeOption& opt) {
	if(opt == enlargeOption::bottom) {
		for(int x=0; x<save.board.size(); ++x) {
			save.board[x].insert(save.board[x].begin(), cell());
		}
	}
	else if(opt == enlargeOption::top) {
		for(int x=0; x<save.board.size(); ++x) {
			save.board[x].push_back(cell());
		}
	}
	else if(opt == enlargeOption::left) {
		save.board.insert(save.board.begin(), column_t());
	}
	else if(opt == enlargeOption::right) {
		save.board.push_back(column_t());
	}
}

void step() {
	do {
		boardChanged = false;

		for(int x=0; x<save.board.size(); ++x) {
			for(int y=0; y<save.board[0].size(); ++y) {
				if(save.board[x][y].prio > 0) doMerge(x, y);
			}
		}
		applyUpdate();
		distribute(save.toString());
	}
	while(boardChanged);
}

// TODO: select STAIR_PRIO_AFTER_MERGE or DEFAULT_PRIO based on whether the stair rule is enabled by the user
// TODO: decide if the user shall be granted such a power, since he is usually stupid
void doMerge(int x, int y) {
	// we need these more than once
	bool leftMerge = x > 0 && save.board[x-1][y].num == save.board[x][y].num;
	bool rightMerge = x < save.board.size() - 1 && save.board[x+1][y].num == save.board[x][y].num;
	bool belowMerge = y > 0 && save.board[x][y-1].num == save.board[x][y].num;
	int cellCount = 1;

	if(leftMerge || rightMerge) { // DEF2 merge (stair rule might apply)
		// the infamous stair rule. yep, thats all.
		// if the priority of a targeted cell is bigger, abort this merge
		if((leftMerge && save.board[x-1][y].prio > save.board[x][y].prio) || (rightMerge && save.board[x+1][y].prio > save.board[x][y].prio)) return;

		save.board[x][y].futureNum = save.board[x][y].num; // set futureNum for later increasing, could be 0 if recently fallen

		if(leftMerge) {
			save.board[x-1][y].futureNum = 0;
			save.board[x-1][y].futurePrio = 0;
			save.board[x][y].futureNum++;
			cellCount++;
		}
		if(rightMerge) {
			save.board[x+1][y].futureNum = 0;
			save.board[x+1][y].futurePrio = 0;
			save.board[x][y].futureNum++;
			cellCount++;
		}
		if(belowMerge) {
			save.board[x][y-1].futureNum = 0;
			save.board[x][y-1].futurePrio = 0;
			save.board[x][y].futureNum++;
			cellCount++;
		}

		save.board[x][y].futurePrio = STAIR_PRIO_AFTER_MERGE; // TODO: or DEFAULT_PRIO if the Stair Rule is disabled
		boardChanged = true;
	}
	else if(belowMerge) { // DEF1 merge (immediately move down, as proclaimed by the Holy Song of Combined Potency)
		save.board[x][y-1].futureNum = save.board[x][y].futureNum + 1;
		save.board[x][y-1].futurePrio = STAIR_PRIO_AFTER_MERGE; // TODO: or DEFAULT_PRIO if the Stair Rule is disabled
		save.board[x][y].futureNum = 0;
		save.board[x][y].futurePrio = 0;
		boardChanged = true;
		cellCount++;
	}
	
	if(boardChanged) {
		int score = (int) pow(2, (save.board[x][y].num + cellCount - 1));
		save.score += score;
		save.maxScore += score;
	}
}

void applyUpdate() {
	for(int x=0; x<save.board.size(); ++x) {
		for(int y=0; y<save.board[0].size(); ++y) {
			if(y < save.board.size() - 1 && save.board[x][y].num == 0 && save.board[x][y+1].num > 0) { // cell has something above and is 0 (NOT will be 0!!)
				// move the cell above to here and reset it
				save.board[x][y] = save.board[x][y+1];
				save.board[x][y].prio = save.board[x][y+1].prio > 1 ? save.board[x][y].prio : DEFAULT_PRIO; // the cell above had a higher prio; copy it
				save.board[x][y+1] = cell();
				boardChanged = true;
			}
			else { // normal update
				save.board[x][y].num = save.board[x][y].futureNum;
				save.board[x][y].prio = save.board[x][y].futurePrio;

				if(save.board[x][y].futurePrio > 0) { // reduce prio if neccessary
					save.board[x][y].futurePrio--;
					boardChanged = true;
				}
			}
		}
	}
}

// needed for Clear event (reaching number E = 14) processing
void clearLowerThan(int num) {
	for(int x=0; x<save.board.size(); ++x) {
		for(int y=0; y<save.board[0].size(); ++y) {
			if(save.board[x][y].num < num) { // reset this cell
				save.board[x][y] = cell();
			}
		}
	}
}

bool insert(int col) {
	if(col < 0 || col >= save.board.size()) {
		save.currentNum = nextNum(6);
		return true; // insert not possible, but no death
	}

	if(save.board[col][save.board[0].size()-1].num != 0) return false; // topmost row is full; can't insert cell

	for(int y=save.board[col].size()-1; y>0; --y) { // scan downwards
		if(save.board[col][y-1].num != 0) { // cell below is not zero

			save.board[col][y].num = save.currentNum;
			save.board[col][y].prio = DEFAULT_PRIO;
			save.board[col][y].futureNum = save.currentNum;
			
			save.currentNum = nextNum(6);
			step();
			return true;
		}
	}

	// we are at the bottom
	save.board[col][0].num = save.currentNum;
	save.board[col][0].prio = DEFAULT_PRIO;
	save.board[col][0].futureNum = save.currentNum;
	
	save.currentNum = nextNum(6);
	step();
	return true;
}

bool buy(int number) {
	if(number < 0) return false;

	int cost = (int) pow(2, (number+3));
	if(save.score < cost) return false;

	save.currentNum = number;
	save.score -= cost;
	return true;
}
