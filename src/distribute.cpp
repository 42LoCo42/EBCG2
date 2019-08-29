#include <vector>
#include <MSS.hpp>

#include "distribute.hpp"
extern MSS server;

void distribute(const std::string& msg) {
	std::vector<int> ids;
	server.getActiveClients(ids);

	for(int client_id : ids) {
		server.send(msg, client_id);
	}
}
