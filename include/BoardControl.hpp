#pragma once

#include "defs.hpp"

/** the possible directions to enlarge() in */
enum enlargeOption {
	top, bottom, left, right
};

void printBoard();
void setupBoard(board_t& board, int xLen, int yLen); /** creates an empty board of these dimensions */
void enlarge(const enlargeOption& opt); /** adds a row of cells on this side */
void step(); /** checks all cells for merges and does applyUpdate */
void doMerge(int x, int y); /** check a cell for a merge and set its future values */
void applyUpdate(); /** for each cell, copies the future values into the current ones */
void clearLowerThan(int num); /** delete all cells with their num lower than this one */
bool insert(int num, int col); /** insert a cell with this num into that column */
