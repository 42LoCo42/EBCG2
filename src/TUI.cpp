#include <ncurses.h>
#include <CursedCat.hpp>
#include <string>
#include <vector>

#include "TUI.hpp"
#include "defs.hpp"
#include "SaveState.hpp"

using namespace std;
using namespace CursedCat;

extern SaveState currentSave;

void startTUI() {
	initscr();
	raw(); // for function keys
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	if(has_colors) {
		start_color();
		init_pair(1, COLOR_GREEN, COLOR_BLACK); // default cell
		init_pair(2, COLOR_RED, COLOR_BLACK); // active cell
		init_pair(3, COLOR_BLACK, COLOR_YELLOW); // blocked cell
		init_pair(4, COLOR_RED, COLOR_YELLOW); // active & blocked
	}

	box(stdscr, 0, 0);
}

void printGameState(GameState& gs) {
	int height, width;
	getmaxyx(stdscr, height, width);

	if(gs == GameState::mainMenu) {
		vector<LineData> lines = CursedCat::read("assets/mainMenu.txt");

		int selected = handleMenu(vector<LineData> {lines[23], lines[24], lines[25], lines[26]});
		switch(selected) {
			case 0: gs = GameState::playing; break;
			case 1: gs = GameState::openMenu; break;
			case 2: gs = GameState::settingsMenu; break;
			case 3: gs = GameState::quit; break;
		}
	}
	else if(gs == GameState::openMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::settingsMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::playing) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::pauseMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::saveMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::shop) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::timeline) {
		gs = GameState::mainMenu;
	}
}

int handleMenu(const vector<LineData>& lines) {
	int selected = 0;
	int c;

	while(true) {
		for(int i=0; i<lines.size(); ++i) {
			mvprintw(lines[i].y, lines[i].x, " ");
			mvprintw(lines[i].y, lines[i].x + lines[i].length - 1, " ");
		}

		attron(COLOR_PAIR(1));
		mvprintw(lines[selected].y, lines[selected].x, "[");
		mvprintw(lines[selected].y, lines[selected].x + lines[selected].length - 1, "]");
		attroff(COLOR_PAIR(1));

		c = getch();
		if(c == KEY_DOWN && selected < lines.size() - 1) {
			++selected;
		}
		else if(c == KEY_UP && selected > 0) {
			--selected;
		}
		else if(c == 10) { // 10 is the ENTER key
			return selected;
		}
	}
}
