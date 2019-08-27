#include <stdio.h> // printf
#include <string> // for message type
#include <MSS.hpp> // for server

#include "defs.hpp"

using namespace std;

constexpr int DEFAULT_PORT = 2048;

int port = DEFAULT_PORT, clientCount;
GameState gameState = GameState::mainMenu;

int main(int argc, char* argv[]) {
	// get number of clients
	if(argc < 2) {
		printf("Usage: ebcg2 <client count> [port]\n");
		exit(1);
	}

	clientCount = stoi(string(argv[1]));
	if(argc >= 3) port = stoi(string(argv[2]));

	if(clientCount < 1) {
		printf("Error: < 1 clients!\n");
		exit(1);
	}

	// start server
	printf("EBCG2 server started with %i slots on port %i!\n", clientCount, port);
	MSS server = MSS(clientCount, port);
	MSS_msg msg;
	string clientIP;

	// main loop. accept messages from clients, process them and distribute results
	while(true) {
		server.receive(msg);

		// Handle message
		if(msg.text.length() > 0) { // real message		
			printf("%i: %s", msg.client_id, msg.text.c_str());
			server.send(msg.text, msg.client_id);
		}
		else {
			if(msg.isNewClient) { // connection from new client
				server.getClientIP(msg.client_id, clientIP);

				if(msg.client_id == -1) { // no slot for client
					printf("Client %s failed to connect!\n", clientIP.c_str());
				}
				else {
					printf("Client %s (index %i) connected!\n", clientIP.c_str(), msg.client_id);
					server.send("Hello, comrade!\n", msg.client_id);
				}
			}
			else { // client disconnected
				printf("Client %s (index %i) disconnected!\n", clientIP.c_str(), msg.client_id);
			}
		}

	}

	return 0;
}
