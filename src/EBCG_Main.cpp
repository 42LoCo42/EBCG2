#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>

#include "EBCG_Main.hpp"
#include "queue.hpp"
#include "BoardControl.hpp"
#include "TUI.hpp"
#include "defs.hpp"

using namespace std;

bool programActive = true;
mutex programActive_mutex;
bool noTUI = false;
int maxClients = 0;

GameState gameState = GameState::mainMenu;

int main(int argc, char* argv[]) {
	// parse args. i=1 to skip the program invocation
	for(int i=1; i<argc; ++i) {
		string s {argv[i]};
		if(s.compare("--noTUI") == 0) {
			noTUI = true;
		}
		else if(s.compare("--slots") == 0) {
			++i;
			maxClients = stoi(string(argv[i]));
			printf("Server started with %i slots!\n", maxClients);
		}
		else {
			printf("Unknown argument %s\n", s.c_str());
		}
	}

	// create TUI and server thread if needed
	queue<string> msgQueue;
	thread tui_thread;
	thread server_thread;

	if(!noTUI) tui_thread = thread(TUIThread, ref(msgQueue));
	if(maxClients > 0) server_thread = thread(serverThread, ref(msgQueue));

	// main loop. handle messages from TUI and server
	string threadMsg;
	while(programActive) {
		threadMsg = msgQueue.pop();
	}

	// handle thread termination
	if(!noTUI) tui_thread.join();
	if(maxClients > 0) server_thread.join();
	return 0;
}

void TUIThread(queue<string>& msgQueue) {
	startTUI(); // inits ncurses

	while(programActive) {
		printGameState(gameState); // prints everything required
		msgQueue.push(to_string(gameState));
		if(gameState == GameState::quit) break;
	}

	// Program is ending
	endwin();
	lock_guard<mutex> lock(programActive_mutex);
	programActive = false;
	msgQueue.push("_QUIT_"); // unblock main loop
}

void serverThread(queue<string>& msgQueue) {

}
