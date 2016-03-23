

#ifndef __GAMESERVER_HPP__
#define __GAMESERVER_HPP__

#include "msg/MessageDispatcher.hpp"
#include "typedefs.hpp"
#include <memory>
#include "Module.hpp"
#include "msg/MessageFactory.hpp"
#include "ServerMessage.pb.h"

namespace odb {
class database;
}

namespace db {
using DBServer = ::odb::database;
}

class LogServer;

class GameServer {
public:
	using StandardMessageDispatcher =
		msg::MessageDispatcher<msg::ClientMessage,
							   msg::ClientMessage::ClientMessageType,
							   msg::pbmsg_type, WSConnection>;
	using StandardServerMessageFactory =
		msg::MessageFactory<msg::ServerMessage>;
	using StandardSendMessage =
		typename StandardServerMessageFactory::RecycleMessage;
	using StandardSendFunction =
		std::function<void(const StandardSendMessage&, WSConnection&)>;
	using StandardServerMessageFactoryFunction =
		std::function<StandardSendMessage()>;
	using StandardModule =
		Module<StandardMessageDispatcher, StandardSendFunction,
			   StandardServerMessageFactoryFunction>;
	using Database_type = db::DBServer;
	using Database_ptr = std::unique_ptr<Database_type>;
	using LogServer_type = LogServer;
	using ConnectionId = WSRawConnection*;

	GameServer(WSServer& server, LogServer& logServer);
	~GameServer();
	GameServer(const GameServer& other) = delete;

	void start();
	Database_type* getDB();
	LogServer_type* getLogServer();

private:
	struct GameServerImpl;

public:
	using Impl = GameServerImpl;

private:
	std::unique_ptr<GameServerImpl> m_pimpl;

	void forwardModule(std::unique_ptr<StandardModule>);

public:
	inline void loadModule(std::unique_ptr<StandardModule> module) {
		forwardModule(std::move(module));
	}

	template <typename module_T, typename... args_T>
	void emplaceModule(args_T&&... args) {
		forwardModule(std::move(
			std::make_unique<module_T>(std::forward<args_T>(args)...)));
	}

};  // end class GameServer

#endif
