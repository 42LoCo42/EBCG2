#include <iostream>
#include <string>

#include "BoardControl.hpp"
#include "SaveState.hpp"
#include "TUI.hpp"
#include "defs.hpp"

using namespace std;

bool noTUI = false;
int ipcCount = 0;

GameState gameState = GameState::mainMenu;
SaveState currentSave;
board_t board;
int score;

int main(int argc, char* argv[]) {
	// parse args. i=1 to skip the program invocation
	for(int i=1; i<argc; ++i) {
		string s {argv[i]};
		if(s.compare("--noTUI") == 0) {
			noTUI = true;
		}
		else if(s.compare("--ipcCount") == 0) {
			++i;
			ipcCount = stoi(string(argv[i]));
			cout << "Loaded " << ipcCount << " IPC slots!\n";
		}
		else {
			cout << "Unknown argument " << s << '\n';
		}
	}

	// TODO: clean this for IPC init
	currentSave = SaveState(3, 3);
	board = currentSave.board;
	score = currentSave.score;
	// cout << "loaded save\n";
	// printBoard();

	// start the TUI and TODO: its message adapter
	if(!noTUI) {
		startTUI();
		while(true) {
			printGameState(gameState);
			refresh();
			if(gameState == GameState::quit) break;
		}
		endwin();
	}

	return 0;
}
