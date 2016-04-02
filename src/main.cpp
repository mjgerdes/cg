

#include <sstream>

#include "websock/client_ws.hpp"
#include "typedefs.hpp"
#include "GameServer.hpp"
#include "LogServer.hpp"
#include "Module/AuthModule.hpp"
#include "Module/DataModule.hpp"
#include "NewPlayerInitializer.hpp"

using namespace std;

int main(int argc, char** argv) {
	// WebSocket (WS)-server at port 8080 using 4 threads
	unsigned short port = 8080;
	auto threads = 4u;
	WSServer server{port, threads};
	auto logServer = LogServer{true};
	logServer.log<Log::net>("Logging started");

	auto gameServer = std::make_unique<GameServer>(server, logServer);
	auto authModule = std::make_unique<AuthModule>(gameServer->getDB(),
												   gameServer->getLogServer());

	auto dataModule =
		std::make_unique<DataModule>(&(*authModule), gameServer->getDB(),
									 "/home/marius/cg/serverapp/data/cards",
									 "/home/marius/cg/serverapp/data/systems",
									 "/home/marius/cg/serverapp/data/hulls");
	authModule->setNewPlayerCallback(
		NewPlayerInitializer{gameServer->getDB(), dataModule->getCardProvider(),
					dataModule->getSystemProvider(), dataModule->getHullProvider()});
	gameServer->loadModule(std::move(authModule));
	gameServer->loadModule(std::move(dataModule));
	//	gameServer->emplaceModule<AuthModule>(gameServer->getDB(),
	// gameServer->getLogServer());

	thread log_thread([&logServer]() { logServer.start(); });

	thread server_thread([&server, &logServer, port, threads]() {
		// Start WS-server
		logServer.log<Log::net>("WebSock server listening on port ", port,
								"with ", threads, "threads");
		server.start();
	});

	gameServer->start();

	server_thread.join();
	log_thread.join();
	return 0;
}
