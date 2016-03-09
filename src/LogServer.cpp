
#include <string>
#include <iostream>
#include <algorithm>
#include <mutex>
#include "boost/lockfree/queue.hpp"

#include "LogServer.hpp"

struct LogServer::LogServerImpl {
	LogServerImpl() : m_messageQueue(1024) {}

public:
	
	typedef LogMessage queue_element_type;
	typedef boost::lockfree::queue<queue_element_type*> queue_type;
	queue_type m_messageQueue;

	std::mutex m_stdoutMutex;
	void enqueue(queue_element_type* msg) {
		while (!m_messageQueue.push(msg))
			;
	}

	void listen() {
		queue_element_type* msg(nullptr);
		while (true) {
			if (m_messageQueue.pop(msg)) {
				msg->write();
				delete msg;
			}
		}  // while
	}

};  // end struct LogServerImpl

// Definitions of LogServer public interface
// constructors
LogServer::LogServer(bool debug) : m_pimpl(std::make_unique<LogServerImpl>()), m_debug(debug) {}
LogServer::~LogServer() {}
LogServer::LogServer(LogServer&& other) : m_pimpl{std::move(other.m_pimpl)} {}

void LogServer::network(const std::string& msg) {
	m_pimpl->enqueue(new Log::NetworkMessage(msg));
}

void LogServer::start() { m_pimpl->listen(); }

void LogServer::forwardEnqueue(LogMessage* msg) { m_pimpl->enqueue(msg); }
