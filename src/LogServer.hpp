
#ifndef __LOGSERVER_HPP__
#define __LOGSERVER_HPP__

#include "Utility.hpp"

class LogServer {
public:
	struct LogMessage {
		LogMessage(const std::string& msg) : m_msg(msg) {}
		virtual ~LogMessage() = default;
		inline virtual void
		write() = 0;  //{ std::cout << m_msg << std::endl; }
	protected:
		const std::string m_msg;
	};

public:
	LogServer();
	~LogServer();
	LogServer(LogServer&& other);

	/*! Starts the logging server and keeps listening for log messages. */
	void start();

	void network(const std::string& msg);

private:
	struct LogServerImpl;
	typedef LogServerImpl Impl;

	std::unique_ptr<LogServerImpl> m_pimpl;

	// have to declare here to forward arguments to implementation with
	// template
	void forwardEnqueue(LogMessage* msg);

public:
	// Template Utility Functions
	template <typename msg_T, typename... args_T>
	void log(args_T&&... args) {
		std::ostringstream msgText;
		Utility::addToStream(msgText, std::forward<args_T>(args)...);
		forwardEnqueue(new msg_T(msgText.str()));
	}

};  // end class Log

namespace Log {
struct NetworkMessage : LogServer::LogMessage {
	NetworkMessage(const std::string& msg) : LogMessage(msg) {}
	inline virtual void write() override {
		std::cout << "[NET] " << m_msg << std::endl;
	}
};

using net = NetworkMessage;
}

#endif
