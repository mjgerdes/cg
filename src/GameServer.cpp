

#include <set>
#include <mutex>
#include "typedefs.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <functional>

#include "GameServer.hpp"
#include "Utility.hpp"
#include "ClientMessage.pb.h"
#include "Registration.pb.h"
#include "msg/MessageExtractor.hpp"
#include "msg/MessageDispatcher.hpp"
#include "Concurrent/Queue.hpp"
#include "msg/MessageFactory.hpp"
#include "db/PlayerAccount.hpp"
#include "database.hpp"
#include "PlayerAccount_odb.h"
using namespace Log;
using namespace std::placeholders;
using namespace db;
std::string connectionString(const WSConnection& con) {
	return Utility::concat(con->remote_endpoint_address, ":",
						   con->remote_endpoint_port, " (", (size_t)con.get(),
						   ")");
}

struct GameServer::GameServerImpl {
public:
	using Impl = GameServer::GameServerImpl;

	WSServer& server;
	LogServer& logServer;
	db::DBServer dbServer;

	/*! Set of connections that have connected to the server, but
	  not been authenticated. */
	std::set<WSConnection> m_unauthedConnections;

	/*! Mutex for unauthed connections. */
	std::mutex m_unauthedConnectionsMutex;

	/*! Map from connections to authenticated Player's accounts. */
	//	std::map<WSConnection, std::shared_ptr<PlayerAccount>>
	// m_authedConnections;
	using PlayerAccount_ptr = std::unique_ptr<PlayerAccount>;
	using PlayerAccountConnections = std::map<WSConnection, PlayerAccount_ptr>;

	PlayerAccountConnections m_connections;

	enum ConnectionStatus { unauthed = 0, authed = 1 };
	using clientMessageFactory_type = msg::MessageFactory<msg::ClientMessage>;
	clientMessageFactory_type m_clientMessageFactory;

	//	using ClientMessage_ptr = std::shared_ptr<msg::ClientMessage>;
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

	msg::MessageDispatcher<msg::ClientMessage,
						   msg::ClientMessage::ClientMessageType,
						   msg::pbmsg_type, WSConnection> m_dispatcher;

public:
	GameServerImpl(WSServer& serv, LogServer& logServ)
		: server(serv),
		  logServer(logServ),
		  dbServer(db::makeDatabaseServer()),
		  m_clientMessageFactory(1) {
		init();
	}

	template <typename msg_T, typename... args_T>
	void log(args_T&&... args) {
		logServer.log<msg_T>(std::forward<args_T>(args)...);
	}

	template <typename... args_T>
	void sendRaw(const WSConnection& reciever, args_T&&... args) {
		const auto& fullString = Utility::concat(std::forward<args_T>(args)...);
		auto msg = std::make_unique<WSServer::SendStream>();
		*msg << fullString;
		log<net>("Sent message to ", connectionString(reciever), "\n",
				 fullString);
		server.send(reciever, std::move(msg));
	}

	void unregisterConnection(WSConnection connection) {
		std::lock_guard<std::mutex> lock(m_unauthedConnectionsMutex);
		m_unauthedConnections.erase(connection);
	}

	void registerConnection(const WSConnection& connection) {
		{
			std::lock_guard<std::mutex> lock(m_unauthedConnectionsMutex);
			m_unauthedConnections.insert(connection);
		}
		log<net>("Initiating login handshake with ",
				 connectionString(connection));
		sendRaw(connection, "AUTHPLS");
	}

	void onCloseEvent(WSConnection connection, int status,
					  const std::string& reason) {
		unregisterConnection(connection);
		log<net>("Closed connection to ", connectionString(connection),
				 " with status code ", status);
	}

	void onMessageEvent(WSConnection connection, WSMessage serverMessage) {
		const auto msg(serverMessage->string());
		log<dbg>("Recieved message from ", connectionString(connection), "\n",
				 msg);

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

	ConnectionStatus connectionStatusOf(const WSConnection& connection) {
		if (m_connections.find(connection) != m_connections.cend()) {
			return authed;
		}

		if (m_unauthedConnections.find(connection) ==
			m_unauthedConnections.cend()) {
			// this really should not happen, but im leaving the check for
			// development
			log<dbg>(
				"error in connectionStatusOf: Invalid state of connection: "
				"connection exists but is not tracked by the server. Source: ",
				connectionString(connection));
		}
		return unauthed;
	}  // end connectionStatusOf

	void onLogin(const msg::Login* msg, const WSConnection source) {
		log<dbg>("Hello to ", msg->email());
		if (connectionStatusOf(source) == authed) {
			log<net>("Player `", msg->email(), "` is already connected: ",
					 connectionString(source));
			// send loginFailure response msg
			return;
		}

		using query = odb::query<db::PlayerAccount>;

		auto transaction = dbServer->begin();
		auto player = PlayerAccount_ptr{dbServer->query_one<db::PlayerAccount>(
			query::email == msg->email())};
		transaction->commit();

		if (!player) {
			log<net>("Invalid login from player `", msg->email(),
					 "` with connection: ", connectionString(source));
			// send invalidLogin message
			return;
		}

		m_connections[source] = std::move(player);

	}  // end onLogin

	void onRegistration(const msg::Registration* msg, WSConnection source) {
		log<net>("Registering player from connection: ",
				 connectionString(source));

		if (connectionStatusOf(source) != unauthed) {
			log<net>("Registration attempt from an already registered User : ",
					 connectionString(source));
			// send invalid registration msg
		}

		using query = odb::query<db::PlayerAccount>;
		{
			odb::transaction t(dbServer->begin());
			auto maybePlayer =
				PlayerAccount_ptr{dbServer->query_one<db::PlayerAccount>(
					query::email == msg->email())};
			t.commit();

			if (maybePlayer) {
				log<net>("Invalid registration attempt; player `", msg->email(),
						 "` already exists. From connection: ",
						 connectionString(source));
				// send invalid registration msg
				return;
			}
		}

		auto newPlayer =
			std::make_unique<PlayerAccount>(msg->email(), msg->password());

		odb::transaction insertTransaction(dbServer->begin());
		dbServer->persist(*newPlayer);
		insertTransaction.commit();

		m_unauthedConnections.erase(source);
		m_connections[source] = std::move(newPlayer);
		log<net>("Player `", msg->email(), " successfully registered");
	}  // end onRegistration

	void init() {
		log<dbg>("Initializing GameServer\n");

		using namespace msg;
		auto reg = m_dispatcher.getRegisterFunction(this);
		reg.entry<Login>(ClientMessage::LoginType, &ClientMessage::login,
						 &Impl::onLogin);
		reg.entry<Registration>(ClientMessage::RegistrationType,
								&ClientMessage::registration,
								&Impl::onRegistration);
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

	void listen() {
		while (true) {
			// this will block
			auto e = m_messageQueue.pop();
			{
				std::lock_guard<std::mutex> msgLock(e.msg.getMutex());
				m_dispatcher.handle(e.msg->msgtype(), &(*(e.msg)), e.source);
			}  // guard
		}	  // while
	}
};  // end struct GameServerImpl

GameServer::GameServer(WSServer& server, LogServer& logServer)
	: m_pimpl(std::make_unique<GameServer::Impl>(server, logServer)) {}
GameServer::~GameServer() {}

void GameServer::start() { m_pimpl->listen(); }

void GameServer::registerConnection(const WSConnection& connection) {
	m_pimpl->registerConnection(connection);
}
