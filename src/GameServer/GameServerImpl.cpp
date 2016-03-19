
#include "GameServer.hpp"
#include "GameServer/GameServerImpl.hpp"
#include "NetUtility.hpp"
#include "database.hpp"
#include "ServerMessage.pb.h"

using namespace Utility;
using namespace Log;
using namespace std::placeholders;
using namespace db;
using Impl = GameServer::Impl;

// constructor
Impl::GameServerImpl(WSServer& serv, LogServer& logServ)
	: server(serv),
	  logServer(logServ),
	  dbServer(db::makeDatabaseServer()),
	  m_clientMessageFactory(1),
	  m_serverMessageFactory(1){
	init();
}

void Impl::sendMessage(const msg::ServerMessage* msg, WSConnection& destination) {
	std::string temp;
	msg->SerializeToString(&temp);
	sendRaw(destination, std::move(temp));
} // end sendMessage

void Impl::loadModule(Module_ptr module) {
	module->bindSendFunction(std::bind(&Impl::sendMessage, this, _1, _2));
	module->bindServerMessageFactoryFunction(std::bind(&GameServer::StandardServerMessageFactory::makeRecycleMessage, &m_serverMessageFactory));
	module->bindHandlers(&m_dispatcher);
	m_modules.push_front(std::move(module));
}

void Impl::onOpenEvent(WSConnection connection) {
	logServer.log<net>("Opened connection to", connectionString(connection));
	sendRaw(connection, "AUTHPLS");


	// send internal connection message
	auto msg = m_clientMessageFactory.makeRecycleMessage();
	msg->set_msgtype(msg::ClientMessage::ConnectType);
	m_messageQueue.emplace(connection, std::move(msg));
}

void Impl::onCloseEvent(WSConnection connection, int status,
						const std::string& reason) {
	log<net>("Closed connection to ", connectionString(connection),
			 " with status code ", status);

	// send internal disconnect message
	auto msg = m_clientMessageFactory.makeRecycleMessage();
	msg->set_msgtype(msg::ClientMessage::DisconnectType);
	m_messageQueue.emplace(connection, std::move(msg));
}

void Impl::onMessageEvent(WSConnection connection, WSMessage serverMessage) {
	const auto msg(serverMessage->string());
	log<dbg>("Recieved message from ", connectionString(connection), "\n", msg);

	auto cmsg = m_clientMessageFactory.makeRecycleMessage();
	{
		std::lock_guard<std::mutex> cmsgLock(cmsg.getMutex());
		if (!cmsg->ParseFromString(msg)) {
			std::cerr << "Could not parse LOL\n";
			return;
		}
	}  // guard
	m_messageQueue.emplace(connection, std::move(cmsg));
}

void Impl::init() {
	log<dbg>("Initializing GameServer\n");
	auto& index = server.endpoint["^/index/?$"];

	index.onopen = std::bind(&Impl::onOpenEvent, this, _1);
	index.onclose = std::bind(&Impl::onCloseEvent, &(*this), _1, _2, _3);
	index.onmessage = std::bind(&Impl::onMessageEvent, &(*this), _1, _2);
}

void Impl::listen() {
	while (true) {
		// this will block
		auto e = m_messageQueue.pop();
		{
			std::lock_guard<std::mutex> msgLock(e.msg.getMutex());
			m_dispatcher.handle(e.msg->msgtype(), &(*(e.msg)), e.source);
		}  // guard
	}	  // while
}
