
#ifndef __AUTHMODULE_HPP__
#define __AUTHMODULE_HPP__

#include "GameServer.hpp"
#include "db/PlayerAccount.hpp"

class AuthModule : public GameServer::StandardModule {
public:
	using Base_type = GameServer::StandardModule;
	enum ConnectionStatus { unauthed = 0, authed = 1 };

public:
	AuthModule(GameServer::Database_type* db, GameServer::LogServer_type* ls)
		: dbServer(db), logServer(*ls) {}
	ConnectionStatus connectionStatusOf(const WSConnection& connection);

	void onLogin(const msg::Login* msg, const WSConnection source);
	void onRegistration(const msg::Registration* msg, WSConnection source);

private:
	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

private:
	GameServer::Database_type* dbServer;
	GameServer::LogServer_type& logServer;

	/*! Map from connections to authenticated Player's accounts. */
	using PlayerAccount_ptr = std::unique_ptr<db::PlayerAccount>;
	using PlayerAccountConnections = std::map<WSConnection, PlayerAccount_ptr>;

	PlayerAccountConnections m_connections;

};  // end class AuthModule

#endif
