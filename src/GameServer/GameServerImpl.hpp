

#ifndef __GAMESERVERIMPL_HPP__
#define __GAMESERVERIMPL_HPP__

#include <forward_list>
#include <mutex>
#include "typedefs.hpp"
#include <iostream>
#include <sstream>
#include <functional>
#include "msg/MessageDispatcher.hpp"
#include "Concurrent/Queue.hpp"
#include "msg/MessageFactory.hpp"
#include "LogServer.hpp"
#include "Utility.hpp"
#include "NetUtility.hpp"


namespace msg {
	class ClientMessage;
}

struct GameServer::GameServerImpl {
public:
	using Impl = GameServer::GameServerImpl;
	using net = Log::net;
	using dbg = Log::dbg;
	using ConnectionId = GameServer::ConnectionId;

	WSServer& server;
	GameServer::LogServer_type& logServer;
	GameServer::Database_ptr dbServer;


	using clientMessageFactory_type = msg::MessageFactory<msg::ClientMessage>;
	clientMessageFactory_type m_clientMessageFactory;

	GameServer::StandardServerMessageFactory m_serverMessageFactory;
	
	using ClientMessage_ptr = clientMessageFactory_type::RecycleMessage;
	struct MessageQueueElement {
		MessageQueueElement(const WSConnection& con,
							const ClientMessage_ptr&& m)
			: source(con), msg(std::move(m)) {}
		MessageQueueElement(MessageQueueElement&& other)
			: source(other.source), msg(std::move(other.msg)) {}
		MessageQueueElement(const MessageQueueElement&& other)
			: source(other.source), msg(std::move(other.msg)) {}

		WSConnection source;
		const ClientMessage_ptr msg;
	};  // end MessageQueueElement

	Concurrent::Queue<MessageQueueElement> m_messageQueue;

	using MessageDispatcher_type = GameServer::StandardMessageDispatcher;
	MessageDispatcher_type m_dispatcher;
	using Module_type = GameServer::StandardModule;
	using Module_ptr = std::unique_ptr<Module_type>;

	std::forward_list<Module_ptr> m_modules;

public:
	GameServerImpl(WSServer& serv, LogServer& logServ);
	template <typename msg_T, typename... args_T>
	void log(args_T&&... args) {
		logServer.log<msg_T>(std::forward<args_T>(args)...);
	}

	template <typename... args_T>
	void sendRaw(const WSConnection& reciever, args_T&&... args) {
		const auto& fullString = Utility::concat(std::forward<args_T>(args)...);
		auto msg = std::make_unique<WSServer::SendStream>();
		*msg << fullString;
		log<dbg>("Raw message to ", Utility::connectionString(reciever), "\n",
				 fullString);
		server.send(reciever, std::move(msg));
	}

	void sendMessage(const GameServer::StandardSendMessage& msg, WSConnection& destination);
	void init();
	void loadModule(Module_ptr);

	void onOpenEvent(WSConnection);
	void onCloseEvent(WSConnection connection, int status,
					  const std::string& reason);
	void onMessageEvent(WSConnection connection, WSMessage serverMessage);

	void listen();

};  // end class GameServerImpl

#endif
