
#ifndef __LOGSERVER_HPP__
#define __LOGSERVER_HPP__

#include "Utility.hpp"

class LogServer {
public:
	struct LogMessage {
		LogMessage(const std::string& msg) : m_msg(msg) {}
		virtual ~LogMessage() = default;
		inline virtual void write() = 0;  //{ std::cout << m_msg << std::endl; }
	protected:
		const std::string m_msg;
	};

public:
	LogServer(bool debug);
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
		//		forwardEnqueue(new msg_T(msgText.str()));
		forwardEnqueue(forwardHelperMakeMessage<msg_T>(msgText.str()));
	}

	template <typename msg_T>
	msg_T* forwardHelperMakeMessage(const std::string& msg) {
		return new msg_T(msg);
	}

private:
	bool m_debug;
};  // end class Log

namespace Log {
struct NetworkMessage : LogServer::LogMessage {
	NetworkMessage(const std::string& msg) : LogMessage(msg) {}
	inline virtual void write() override {
		std::cout << "[NET] " << m_msg << std::endl;
	}
};  // end struct NetworkMessage

struct DebugMessage : LogServer::LogMessage {
	DebugMessage(bool debug, const std::string& msg)
		: LogMessage(msg), m_debug(debug) {}
	inline virtual void write() override {
		if (m_debug) {
			std::cout << "[DBG]" << m_msg << std::endl;
		}
	}

private:
	const bool m_debug;
};  // struct DebugMessage
using net = NetworkMessage;
using dbg = DebugMessage;
}  // end namespace log

template <>
inline Log::DebugMessage* LogServer::forwardHelperMakeMessage(
	const std::string& msg) {
	return new Log::DebugMessage(m_debug, msg);
}

#endif
