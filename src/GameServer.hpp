

#ifndef __GAMESERVER_HPP__
#define __GAMESERVER_HPP__


#include <memory>

#include "LogServer.hpp"

class GameServer {
public:
	GameServer(WSServer& server, LogServer& logServer);
	~GameServer();
	GameServer(const GameServer& other) = delete;

	
	void registerConnection(const WSConnection& connection);
	void start();
	
private:
	struct GameServerImpl;
	using Impl = GameServerImpl;

	std::unique_ptr<GameServerImpl> m_pimpl;
}; // end class GameServer		

		


#endif


