
#include "LogServer.hpp"
#include "Module/AuthModule.hpp"
#include "NetUtility.hpp"
#include "database.hpp"
#include "db/PlayerAccount.hpp"
#include "PlayerAccount_odb.h"

using namespace Utility;
using namespace Log;
using namespace db;
// used only in this module
	struct AuthMessage : LogServer::LogMessage {
		AuthMessage(const std::string& msg) : LogServer::LogMessage(msg) {}
		inline virtual void write() override {
			std::cout << "[AUTH] " << m_msg << std::endl;
		}
	}; // end struct Authmessage
	using auth = AuthMessage;


void AuthModule::bindHandlersImp(MessageDispatcher_type* dispatcher) {
	using namespace msg;
	auto reg = dispatcher->getRegisterFunction(this);
	reg.entry<Login>(ClientMessage::LoginType, &ClientMessage::login,
					 &AuthModule::onLogin);
	reg.entry<Registration>(ClientMessage::RegistrationType,
							&ClientMessage::registration,
							&AuthModule::onRegistration);
	reg.entry<Disconnect>(ClientMessage::DisconnectType,
						  &ClientMessage::disconnect,
						  &AuthModule::onDisconnect);
	reg.entry<Connect>(ClientMessage::ConnectType, &ClientMessage::connect,
					   &AuthModule::onConnect);
	reg.entry<LoginToken>(msg::ClientMessage::LoginTokenType,
						  &ClientMessage::login_token,
						  &AuthModule::onLoginToken);
}

void AuthModule::sendLoginResponse(bool wasSuccessful,
								   WSConnection destination) {
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::LoginResponseType);
	msg->mutable_loginresponse()->set_success(wasSuccessful);
	sendMessage(msg, destination);
}  // end sendLoginResponse

void AuthModule::sendRegistrationResponse(bool wasSuccessful,
										  WSConnection destination) {
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::RegistrationResponseType);
	msg->mutable_registrationresponse()->set_success(wasSuccessful);
	sendMessage(msg, destination);
}  // end sendLoginResponse

AuthModule::ConnectionStatus AuthModule::connectionStatusOf(
	const WSConnection& connection) {
	if (m_connections.find(&*(connection)) != m_connections.cend()) {
		return authed;
	}
	return unauthed;
}  // end connectionStatusOf

void AuthModule::setNewPlayerCallback(NewPlayerCallback_type f) {
	m_newPlayerCallback = std::move(f);
}

optional<AuthModule::PlayerId_type> AuthModule::getIdFor(
	WSConnection connection) {
	const auto i = m_connections.find(&(*connection));
	if (i == m_connections.cend()) {
		return optional<PlayerId_type>{};
	}
	return make_optional<PlayerId_type>(i->second->id());
}

void AuthModule::onLogin(const msg::Login* msg, const WSConnection source) {
	logServer.log<dbg>("Hello to ", msg->email());
	if (connectionStatusOf(source) == authed) {
		logServer.log<auth>("Player `", msg->email(), "` is already connected: ",
						   connectionString(source));
		// FIXME: this might not make sense, what if someone is logged in and
		// they log in with different account details
		sendLoginResponse(true, source);
		return;
	}

	using query = odb::query<db::PlayerAccount>;

	odb::transaction transaction(dbServer->begin());
	auto player = PlayerAccount_ptr{
		dbServer->query_one<db::PlayerAccount>(query::email == msg->email())};
	transaction.commit();

	if (!player) {
		logServer.log<auth>("Invalid login from player `", msg->email(),
						   "` with connection: ", connectionString(source));
		sendLoginResponse(false, source);
		return;
	}

	//	m_connections[&(*source)] = std::move(player);
	//	sendLoginResponse(true, source);
	giveAuth(std::move(source), std::move(player));
}  // end onLogin

void AuthModule::onRegistration(const msg::Registration* msg,
								WSConnection source) {
	logServer.log<auth>("Registering player from connection: ",
					   connectionString(source));

	if (connectionStatusOf(source) != unauthed) {
		logServer.log<auth>(
			"Registration attempt from an already registered User : ",
			connectionString(source));
		sendRegistrationResponse(false, source);
		return;
	}

	using query = odb::query<db::PlayerAccount>;
	{
		odb::transaction t(dbServer->begin());
		auto maybePlayer =
			PlayerAccount_ptr{dbServer->query_one<db::PlayerAccount>(
				query::email == msg->email())};
		t.commit();

		if (maybePlayer) {
			logServer.log<auth>("Invalid registration attempt; player `",
							   msg->email(),
							   "` already exists. From connection: ",
							   connectionString(source));
			sendRegistrationResponse(false, source);
			return;
		}
	}

	auto newPlayer =
		std::make_unique<db::PlayerAccount>(msg->email(), msg->password());

	odb::transaction insertTransaction(dbServer->begin());
	dbServer->persist(*newPlayer);
	insertTransaction.commit();
	// fire new player callback for further initialization
	m_newPlayerCallback(newPlayer->id(), source);

//	m_connections[&(*source)] = std::move(newPlayer);
	giveAuth(source, std::move(newPlayer));
	// send successful register response
	// (might not need this)
//	sendRegistrationResponse(true, source);
	logServer.log<auth>("Player `", msg->email(), " successfully registered");
}  // end onRegistration

void AuthModule::onDisconnect(const msg::Disconnect* msg,
							  WSConnection connection) {
	if (connectionStatusOf(connection) == authed) {
		logServer.log<auth>("Disconnect; releasing auth for connection ",
						   Utility::connectionString(connection));
		m_connections.erase(&(*connection));
	}
}  // end onDisconnect

void AuthModule::onConnect(const msg::Connect* msg,
						   WSConnection newConnection) {}  // end onConnect

void AuthModule::onLoginToken(const msg::LoginToken* msg, WSConnection source) {
	logServer.log<auth>("Recieved login token ", msg->token(), " for user ",
					   msg->email(), " from connection ",
					   connectionString(source));

	const auto idForToken = m_tokens.find(msg->token());
	if (idForToken == m_tokens.cend()) {
		logServer.log<auth>("Invalid or expired login token for user ",
						   msg->email(), " from connection ",
						   connectionString(source));
		sendInvalidTokenResponse(source);
		return;
	}

	{
		odb::transaction t{dbServer->begin()};
		auto account = PlayerAccount_ptr{
			dbServer->load<db::PlayerAccount>(idForToken->second)};
		// sanity check - this really cannot happen
		if (!account) {
			logServer.log<dbg>(
				"Error in AuthModule::onLoginToken: invalid id produced a "
				"nullptr");
			return;
		}

		// possible attack
		if (account->email() != msg->email()) {
			logServer.log<auth>(
				"Email/id mismatch for token from user ", msg->email(),
				", tried to authenticate for token for user ", account->email(),
				"; connection ", connectionString(source));
			sendInvalidTokenResponse(source);
			return;
		}

		giveAuth(std::move(source), std::move(account));
	}
}

void AuthModule::sendLoginTokenIssue(token_type token,
									 WSConnection destination) {
	logServer.log<auth>("Handing out login token to ", connectionString(destination));
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::LoginTokenIssueType);
	msg->mutable_login_token_issue()->set_token(token);
	sendMessage(msg, destination);
}

void AuthModule::sendInvalidTokenResponse(WSConnection destination) {
	logServer.log<auth>("Sending invalid login token response");
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::LoginTokenInvalidResponseType);
	sendMessage(msg, destination);
}
AuthModule::token_type AuthModule::registerTokenFor(PlayerId_type id) {
	m_lastToken += 1;
	m_tokens[m_lastToken] = id;
	return m_lastToken;
}

void AuthModule::giveAuth(WSConnection connection, PlayerAccount_ptr account) {
		sendLoginTokenIssue(registerTokenFor(account->id()), connection);
	m_connections[&(*connection)] = std::move(account);
	sendLoginResponse(true, connection);
}
