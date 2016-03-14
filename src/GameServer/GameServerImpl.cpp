
#include "GameServer.hpp"
#include "GameServer/GameServerImpl.hpp"
#include "NetUtility.hpp"

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
	  m_clientMessageFactory(1) {
	init();
}

void Impl::loadModule(Module_ptr module) {
	module->bindHandlers(&m_dispatcher);
	m_modules.push_front(std::move(module));
}

void Impl::unregisterConnection(WSConnection connection) {}

void Impl::registerConnection(const WSConnection& connection) {
	log<net>("Initiating login handshake with ", connectionString(connection));
	sendRaw(connection, "AUTHPLS");
}

void Impl::onCloseEvent(WSConnection connection, int status,
						const std::string& reason) {
	unregisterConnection(connection);
	log<net>("Closed connection to ", connectionString(connection),
			 " with status code ", status);
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

	using namespace msg;
	//	auto reg = m_dispatcher.getRegisterFunction(this);
	/*
		reg.entry<Login>(ClientMessage::LoginType, &ClientMessage::login,
						 &Impl::onLogin);
		reg.entry<Registration>(ClientMessage::RegistrationType,
								&ClientMessage::registration,
								&Impl::onRegistration);
	*/
	auto& index = server.endpoint["^/index/?$"];

	auto& gameServer(*this);
	auto& logServer(this->logServer);
	index.onopen = [&gameServer, &logServer](
		std::shared_ptr<WSServer::Connection> connection) {
		logServer.log<net>("Opened connection to",
						   connectionString(connection));
		gameServer.registerConnection(connection);
	};
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
