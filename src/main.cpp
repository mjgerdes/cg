

#include <sstream>


#include "websock/client_ws.hpp"
#include "typedefs.hpp"
#include "GameServer.hpp"
#include "LogServer.hpp"
#include "Module/AuthModule.hpp"
#include "CardData.pb.h"
#include "CardProvider.hpp"
using namespace std;

void makeMessage();
int main(int argc, char** argv) {
	makeMessage();
	// WebSocket (WS)-server at port 8080 using 4 threads
	unsigned short port = 8080;
	auto threads = 4u;
	WSServer server{port, threads};
	auto logServer = LogServer{true};
	logServer.log<Log::net>("Logging started");

	auto gameServer = std::make_unique<GameServer>(server, logServer);
	gameServer->emplaceModule<AuthModule>(gameServer->getDB(), gameServer->getLogServer());
	CardProvider cp("/home/marius/cg/serverapp/data/cards");
	thread log_thread([&logServer]() { logServer.start(); });

	thread server_thread([&server, &logServer, port, threads]() {
		// Start WS-server
		logServer.log<Log::net>("WebSock server listening on port ", port, "with ", threads, "threads");
		server.start();
	});

	gameServer->start();

	server_thread.join();
	log_thread.join();
	return 0;
}


#include "google/protobuf/text_format.h"
void makeMessage() {
	using namespace data;
	auto c = CardData{};
	c.set_id(1);
	c.set_name("Missile");
	c.set_type(CardData::military);
	c.set_system_id(0);
	c.set_science_cost(0);
	c.set_military_cost(2);
	c.set_support_cost(0);
	c.set_attack(3);
	c.set_health(2);
	c.set_description("A big missile");
	c.set_fluff("Missiles come in all sorts of sizes.");
	std::string out;
	google::protobuf::TextFormat::PrintToString(c, &out);
	std::cout << out << std::endl;

}
