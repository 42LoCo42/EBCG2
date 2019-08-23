#pragma once
#include <ncurses.h>
#include <CursedCat.hpp>
#include <vector>

#include "defs.hpp"

void startTUI();
void printGameState(GameState& gs);
int handleMenu(const CursedCat::LineDataSet& lds);
