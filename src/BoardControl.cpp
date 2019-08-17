#include "BoardControl.hpp"
#include "defs.hpp"

// connect us to program-wide data
extern board_t board;

bool boardChanged = false; // set if a step() changed the board -> redo step()

#include <cstdio>
void printBoard() {
	for(int y=0; y<board[0].size(); ++y) {
		for(int x=0; x<board.size(); ++x) {
			printf("%i,%i|%i,%i ", board[x][y].num, board[x][y].prio, board[x][y].futureNum, board[x][y].futurePrio);
		}
		printf("\n");
	}
	printf("\n");
}

void setupBoard(board_t& board, int xLen, int yLen) {
	board = board_t(xLen);
	for(int x=0; x<xLen; ++x) {
		board[x] = column_t(yLen); // cells are created automagically
	}
}

void enlarge(const enlargeOption& opt) {
	if(opt == enlargeOption::bottom) {
		for(int x=0; x<board.size(); ++x) {
			board[x].insert(board[x].begin(), cell());
		}
	}
	else if(opt == enlargeOption::top) {
		for(int x=0; x<board.size(); ++x) {
			board[x].push_back(cell());
		}
	}
	else if(opt == enlargeOption::left) {
		board.insert(board.begin(), column_t());
	}
	else if(opt == enlargeOption::right) {
		board.push_back(column_t());
	}
}

void step() {
	do {
		boardChanged = false;

		printf("mergetest\n");
		for(int x=0; x<board.size(); ++x) {
			for(int y=0; y<board[0].size(); ++y) {
				if(board[x][y].prio > 0) doMerge(x, y);
			}
		}
		printBoard();

		printf("update\n");
		applyUpdate();
		printBoard();
	}
	while(boardChanged);
}

// TODO: select STAIR_PRIO_AFTER_MERGE or DEFAULT_PRIO based on whether the stair rule is enabled by the user
// TODO: decide if the user shall be granted such a power, since he is usually stupid
void doMerge(int x, int y) {
	// we need these more than once
	bool leftMerge = x > 0 && board[x-1][y].num == board[x][y].num;
	bool rightMerge = x < board.size() - 1 && board[x+1][y].num == board[x][y].num;
	bool belowMerge = y > 0 && board[x][y-1].num == board[x][y].num;

	if(leftMerge || rightMerge) { // DEF2 merge (stair rule might apply)
		// the infamous stair rule. yep, thats all.
		if((leftMerge && board[x-1][y].prio > board[x][y].prio) || (rightMerge && board[x+1][y].prio > board[x][y].prio)) return;

		board[x][y].futureNum = board[x][y].num; // could be 0 if recently fallen

		if(leftMerge) {
			board[x-1][y].futureNum = 0;
			board[x-1][y].futurePrio = 0;
			board[x][y].futureNum++;
		}
		if(rightMerge) {
			board[x+1][y].futureNum = 0;
			board[x+1][y].futurePrio = 0;
			board[x][y].futureNum++;
		}
		if(belowMerge) {
			board[x][y-1].futureNum = 0;
			board[x][y-1].futurePrio = 0;
			board[x][y].futureNum++;
		}

		board[x][y].futurePrio = STAIR_PRIO_AFTER_MERGE;
		boardChanged = true;
	}
	else if(belowMerge) { // DEF1 merge (immediately move down, as proclaimed by the Holy Song of Combined Potency)
		board[x][y-1].futureNum = board[x][y].futureNum + 1;
		board[x][y-1].futurePrio = STAIR_PRIO_AFTER_MERGE;
		board[x][y].futureNum = 0;
		board[x][y].futurePrio = 0;
		boardChanged = true;
	}
}

void applyUpdate() {
	for(int x=0; x<board.size(); ++x) {
		for(int y=0; y<board[0].size(); ++y) {
			if(y < board.size() - 1 && board[x][y].num == 0 && board[x][y+1].num > 0) { // cell has something above and is 0 (NOT will be 0!!)
				// move the cell above to here and reset it
				board[x][y] = board[x][y+1];
				board[x][y].prio = board[x][y+1].prio > 1 ? board[x][y].prio : DEFAULT_PRIO; // the cell above had a higher prio; copy it
				board[x][y+1] = cell();
				boardChanged = true;
			}
			else { // normal update
				board[x][y].num = board[x][y].futureNum;
				board[x][y].prio = board[x][y].futurePrio;

				if(board[x][y].futurePrio > 0) { // reduce prio if neccessary
					board[x][y].futurePrio--;
					boardChanged = true;
				}
			}
		}
	}
}

void clearLowerThan(int num) {
	for(int x=0; x<board.size(); ++x) {
		for(int y=0; y<board[0].size(); ++y) {
			if(board[x][y].num < num) {
				board[x][y] = cell();
			}
		}
	}
}

bool insert(int num, int col) {
	if(board[col][board[0].size()-1].num != 0) return false; // topmost row is full; can't insert cell

	for(int y=board[col].size()-1; y>0; --y) { // scan downwards
		if(board[col][y-1].num != 0) { // cell below is not zero
			printf("insert\n");

			board[col][y].num = num;
			board[col][y].prio = DEFAULT_PRIO;
			board[col][y].futureNum = num;

			printBoard();
			step();
			return true;
		}
	}

	// we are at the bottom
	printf("insert\n");

	board[col][0].num = num;
	board[col][0].prio = DEFAULT_PRIO;
	board[col][0].futureNum = num;

	printBoard();
	step();
	return true;
}
