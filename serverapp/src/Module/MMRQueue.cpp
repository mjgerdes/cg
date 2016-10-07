
#include <string>
#include "MMRQueue.hpp"

void MMRQueue::run() {
	while(true) {
		// this can block if queue is empty, so we have to check
		if(!m_incoming.empty()) {
	auto e = m_incoming.pop();
	internalEnqueue(e);
		}
		tryMatch();
				using namespace std::literals;
				std::this_thread::sleep_for(3s);		
	} // end while
} // end run

void MMRQueue::enqueue(IncomingElement e) { m_incoming.push(e); }

void MMRQueue::internalEnqueue(const IncomingElement& e) {
	m_mmrqueue.emplace_back(e.first, e.second, 0);
} // end internalEnqueue


void MMRQueue::tryMatch() {
	if(m_mmrqueue.size() < 2) {
		return;
	}
	
} // end tryMatch
MMRQueue::~MMRQueue() {}
