
#include "GameServer.hpp"
#include "Concurrent/Queue.hpp"

#ifndef __MMRQUEUE_HPP__
#define __MMRQUEUE_HPP__

class MMRQueue {
public:
	using MMR = unsigned int;
	using IncomingElement = std::pair<GameServer::ConnectionId, MMR>;
	using WaitTime_type = unsigned int;

	struct MMRQueueElement {
		MMRQueueElement(GameServer::ConnectionId con, MMR r, WaitTime_type t)
			: connection(con), rank(r), waitTime(t) {}

		GameServer::ConnectionId connection;
		MMR rank;
		WaitTime_type waitTime;
	};  // end MMRQueueElement

public:
	void run();
	void enqueue(IncomingElement e);

// need this because of unique_ptr - thanks!
	~MMRQueue();
private:
	Concurrent::Queue<IncomingElement> m_incoming;
	std::vector<MMRQueueElement> m_mmrqueue;
};  // end MMRQueue

#endif