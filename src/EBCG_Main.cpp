#include <stdio.h> // printf
#include <string> // for queue type
#include <thread>
#include <mutex> // safe & synchronized termination
// #include <unistd.h>

#include "queue.hpp"
#include "defs.hpp"

using namespace std;

int clientCount = 0;

mutex programActive_mutex;
bool programActive = true;

queue<string> actionQueue;
queue<string> resultQueue;

GameState gameState = GameState::mainMenu;

int main(int argc, char* argv[]) {
	// get number of clients
	if(argc < 2) {
		printf("Usage: ebcg2 <client count>\n");
		exit(1);
	}

	clientCount = stoi(string(argv[1]));
	if(clientCount < 1) {
		printf("Error: < 1 clients!\n");
		exit(1);
	}

	// start server
	thread serverThread = thread(serverLoop);
	printf("EBCG2 server started with %i slots!\n", clientCount);

	// main loop. accept messages from server, process them and send results back
	string threadMsg;
	while(programActive) {
		threadMsg = actionQueue.pop();
		// TODO: process messages. for now, push temporary result.
		resultQueue.push("done");
	}

	// wait for server to end
	server_thread.join();
	return 0;
}

/** 
TODO
Mutex locking

lock_guard<mutex> lock(programActive_mutex);
programActive = false;
msgQueue.push("_QUIT_"); // unblock main loop
*/
