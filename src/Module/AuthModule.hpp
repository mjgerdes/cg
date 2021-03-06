
#ifndef __AUTHMODULE_HPP__
#define __AUTHMODULE_HPP__

#include <boost/container/flat_map.hpp>
#include <unordered_map>
#include "GameServer.hpp"
#include "db/PlayerAccount.hpp"
#include "optional.hpp"

class AuthModule : public GameServer::StandardModule {
public:
	using Base_type = GameServer::StandardModule;
	using PlayerAccount_type = db::PlayerAccount;
	using PlayerId_type = typename PlayerAccount_type::Id_type;
	using NewPlayerCallback_type =
		std::function<void(PlayerId_type, WSConnection)>;
	using NewPlayerCallback_ptr = std::unique_ptr<NewPlayerCallback_type>;
	enum ConnectionStatus { unauthed = 0, authed = 1, tokenAuth = 2 };

public:
	AuthModule(GameServer::Database_type* db, GameServer::LogServer_type* ls)
		: dbServer(db), logServer(*ls), m_lastToken(0), m_reverseLookupMap() {}

	void setNewPlayerCallback(NewPlayerCallback_type);
	Utility::optional<PlayerId_type> getIdFor(WSConnection) const;
	ConnectionStatus connectionStatusOf(const WSConnection& connection) const;
	ConnectionStatus connectionStatusOf(const GameServer::ConnectionId connection) const;
	Utility::optional<WSConnection> reverseLookup(const GameServer::ConnectionId connection) const;

private:
	using PlayerAccount_ptr = std::unique_ptr<db::PlayerAccount>;
	using token_type = unsigned long;
	using PlayerAccountTokens =
		boost::container::flat_map<token_type, PlayerId_type>;

private:
	// private member functions
	void onLogin(const msg::Login* msg, const WSConnection source);
	void onLoginToken(const msg::LoginToken* msg, WSConnection source);
	void onRegistration(const msg::Registration* msg, WSConnection source);
	void onDisconnect(const msg::Disconnect* msg, WSConnection source);
	void onConnect(const msg::Connect* msg, WSConnection source);

	void sendLoginResponse(bool wasSuccessful, WSConnection destination);
	void sendRegistrationResponse(bool wasSuccessful, WSConnection destination);
	void sendLoginTokenIssue(token_type token, WSConnection destination);
	void sendInvalidTokenResponse(WSConnection destination);
	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

	void giveAuth(WSConnection connection, PlayerAccount_ptr account);
	token_type registerTokenFor(PlayerId_type id);

private:
	// private data members
	GameServer::Database_type* dbServer;
	GameServer::LogServer_type& logServer;

	/*! Map from connections to authenticated Player's accounts. */

	using PlayerAccountConnections =
		boost::container::flat_map<GameServer::ConnectionId, PlayerAccount_ptr>;
	PlayerAccountConnections m_connections;


	NewPlayerCallback_type m_newPlayerCallback;

	/*! Map from session tokens to playerids to allow remember me functionality
	 */
	PlayerAccountTokens m_tokens;

	/*! The last token issued, registerTokenFor automatically increases this */
	token_type m_lastToken;

	// map from connectionIds (just raw pointers) to actual safe shared pointers; this is map so we don't have to keep copying shared pointers on insert/delete
	std::unordered_map<GameServer::ConnectionId, WSConnection> m_reverseLookupMap;

};  // end class AuthModule

#endif
