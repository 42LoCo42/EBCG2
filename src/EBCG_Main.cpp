#include <stdio.h> // printf
#include <string> // for messages
#include <MSS.hpp> // for server
#include <filesystem> // for saving & loading

#include "defs.hpp"
#include "distribute.hpp"
#include "BoardControl.hpp"
#include "SaveState.hpp"

using namespace std;
namespace fs = std::filesystem;

constexpr int DEFAULT_PORT = 2048;
const string saveGameFolder = "./EBCG2_savegames";
int port = DEFAULT_PORT, clientCount;
MSS server;

bool inGame = false;
SaveState save;

void split(const string& s, const string& delim, vector<string>& strings) {
	int last = 0, next = 0;

	while((next = s.find(delim, last)) != string::npos) {
		strings.push_back(s.substr(last, next-last));
		last = next + delim.length();
	}
	strings.push_back(s.substr(last));
}

void sendModeError(int client_id, const string& errorMsg = "") {
	server.send("error WrongMode", client_id);
	printf("Mode error! %s\n", errorMsg.c_str());
}

int parseInt(const string& s) {
	try {
		return stoi(s);
	} catch(exception& e) {
		return 0;
	}
}

void checkSaveGameFolder() {
	if(!fs::exists(saveGameFolder)) {
		fs::create_directory(saveGameFolder);
	}
}

int main(int argc, char* argv[]) {
	// get number of clients
	if(argc < 2) clientCount = 1;
	else clientCount = stoi(string(argv[1]));

	// get port
	if(argc < 3) port = DEFAULT_PORT;
	else port = stoi(string(argv[2]));

	if(clientCount < 1) {
		printf("Error: < 1 clients!\n");
		exit(1);
	}

	// start server
	server.init(clientCount, port);
	printf("EBCG2 server started with %i slots on port %i!\n", clientCount, port);

	MSS_msg msg;
	string clientIP;
	string command;

	// main loop. accept messages from clients, process them and distribute results
	while(true) {
		server.receive(msg);
		server.getClientIP(msg.client_id, clientIP);

		// Handle message
		if(msg.text.length() > 0) { // real message		
			printf("Client %s (index %i): %s\n", clientIP.c_str(), msg.client_id, msg.text.c_str());
			vector<string> parts;
			split(msg.text, " ", parts);
			command = parts[0];

			// Game controls
			if(command.compare("newgame") == 0) {
				if(inGame) sendModeError(msg.client_id, "In game - not restarting!");
				else {
					inGame = true;
					printf("Starting new game!\n");
					save.init();
					distribute(save.toString());
				}
			}
			else if(command.compare("savegame") == 0) {distribute("WIP");}
			else if(command.compare("listsavegames") == 0) {
				checkSaveGameFolder();
				for(const auto& entry : fs::directory_iterator(saveGameFolder)) {
					// TODO
				}
			}
			else if(command.compare("loadgame") == 0) {distribute("WIP");}
			else if(command.compare("quitgame") == 0) {
				if(!inGame) sendModeError(msg.client_id, "Not in game - can't quit!");
				else {
					inGame = false;
					save = SaveState();
					printf("Quit game!\n");
				}
			}
			else if(command.compare("stopserver") == 0) {
				printf("Server is stopping!\n");
				distribute("stopped");
				break;
			}
			else if(command.compare("insert") == 0) {
				if(!inGame) sendModeError(msg.client_id, "Can't insert while not in game!");
				else if(parts.size() < 2) {
					server.send("error ArgCount", msg.client_id);
					printf("Too few arguments - need 2!\n");
				}
				else {
					int column = parseInt(parts[1]);
					printf("Inserting %i into column %i!\n", save.currentNum, column);
					insert(column);
					// distribution of save is done in BoardControl.step()
				}
			}
			else if(command.compare("buy") == 0) {
				if(!inGame)	sendModeError(msg.client_id, "Not in game - can't buy!");
				else {
					int num = parseInt(parts[1]);
					if(buy(num)) {
						printf("Bought %i!\n", num);
						distribute(save.toString());
					}
					else {
						printf("Can't buy %i - not enough points!\n", num);
						server.send("error NotEnoughPoints", msg.client_id);
					}
				}
			}
			else if(command.compare("clearoption") == 0) {}
			else if(command.compare("debug") == 0) {
				printf("%s\n", save.toString().c_str());
				printf("x:%i y:%i\n", save.board.size(), save.board[0].size());
			}
			else {
				printf("Invalid command!\n");
				server.send("error Invalid", msg.client_id);
			}
		}
		else {
			if(msg.isNewClient) { // connection from new client
				if(msg.client_id == -1) { // no slot for client
					printf("Client %s failed to connect!\n", clientIP.c_str());
				}
				else {
					printf("Client %s (index %i) connected!\n", clientIP.c_str(), msg.client_id);
					distribute("connect " + to_string(msg.client_id));
				}
			}
			else { // client disconnected
				printf("Client %s (index %i) disconnected!\n", clientIP.c_str(), msg.client_id);
				distribute("disconnect " + to_string(msg.client_id));

			}
		}
	}

	printf("Server has stopped!\n");
	return 0;
}
