
#include <math.h>
#include <algorithm>
#include <string>
#include "MMRQueue.hpp"

MMRQueue::MMRQueue() : m_waitTimeStep(3) {}

void MMRQueue::run() {
	while (true) {
		// this can block if queue is empty, so we have to check
		if (!m_incoming.empty()) {
			auto e = m_incoming.pop();
			internalEnqueue(e);
		}
		tryMatch();
		using namespace std::literals;
		std::this_thread::sleep_for(std::chrono::seconds(m_waitTimeStep));
	}  // end while
}  // end run

void MMRQueue::enqueue(IncomingElement e) { m_incoming.push(e); }

void MMRQueue::internalEnqueue(const IncomingElement& e) {
	m_mmrqueue.emplace_back(e.first, e.second, 0);
}  // end internalEnqueue

void MMRQueue::tryMatch() {
	if (m_mmrqueue.size() < 2) {
		return;
	}
	// the queue has at least 2 members
	// we could sort it here, but leaving it unsorted always keeps

	auto i = m_mmrqueue.begin();
	for (auto j = m_mmrqueue.begin() + 1; j < m_mmrqueue.end(); ++j) {
		auto rankDiff = abs((int)i->rank - (int)j->rank);
		if ((rankDiff ^ 2) <= ((i->waitTime + j->waitTime) / 2)) {
			match(i->connection, j->connection);
			m_mmrqueue.erase(j);
			// j and all iterators > j are now invalidated, luckily i is smaller
			m_mmrqueue.erase(i);
			increaseWaitTime();
			return;
		}
	}
}  // end tryMatch

void MMRQueue::increaseWaitTime() {
// FIXME: this step could be integrated with tryMatch's loop
	// FIXME: we are not even measuring time - currently we just add 3 - value that is a guaranteed wait time after a tryMatch call
	for(auto i = m_mmrqueue.begin(); i < m_mmrqueue.end(); ++i) {
		i->waitTime += m_waitTimeStep; // this can wrap around after a while but oh well
	}
} // end increaseWaitTime

void MMRQueue::match(GameServer::ConnectionId p1, GameServer::ConnectionId p2) {

}

MMRQueue::~MMRQueue() {}
