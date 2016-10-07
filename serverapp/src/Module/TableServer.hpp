

#ifndef __TABLESERVER_HPP__
#define __TABLESERVER_HPP__

#include "GameServer.hpp"
#include "include/Concurrent/Queue.hpp"

class PlayModule;

class TableServer {
public:
	TableServer(PlayModule& parent);

	void enqueueMatch(const GameServer::ConnectionId p1,
					  const GameServer::ConnectionId p2);
void run();

	~TableServer();

private:
	PlayModule& m_parent;

	using QueueMatch_type =
		std::pair<GameServer::ConnectionId, GameServer::ConnectionId>;
	Concurrent::Queue<QueueMatch_type> m_incomingMatches;
};  // end class TableServer

#endif


