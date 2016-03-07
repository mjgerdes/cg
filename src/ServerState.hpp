

#ifndef __SERVERSTATE_HPP__
#define __SERVERSTATE_HPP__

// some necessary forward declarations
//class GameServer;
//class LogServer;

#include "typedefs.hpp"
#include "GameServer.hpp"
#include "LogServer.hpp"


struct ServerState {
public:
	ServerState(WSServer& server, GameServer& gameServer, LogServer& logServer)
		: m_server(server), m_gameServer(gameServer), m_logServer(logServer) {}

	inline WSServer& getServer() { return m_server; }
	inline GameServer& getGameServer() { return m_gameServer; }
	inline LogServer& getLogServer() { return m_logServer; }

		private:
	ServerState() = delete;
	ServerState(const ServerState&) = delete;

	WSServer& m_server;
	GameServer& m_gameServer;
	LogServer& m_logServer;
}; //end struct ServerState

#endif

