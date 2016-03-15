
#include "Module/AuthModule.hpp"
#include "NetUtility.hpp"
#include "db/PlayerAccount.hpp"
#include "PlayerAccount_odb.h"

using namespace Utility;
using namespace Log;
using namespace db;

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
}

AuthModule::ConnectionStatus AuthModule::connectionStatusOf(
	const WSConnection& connection) {
	if (m_connections.find(connection) != m_connections.cend()) {
		return authed;
	}
	return unauthed;
}  // end connectionStatusOf

void AuthModule::onLogin(const msg::Login* msg, const WSConnection source) {
	logServer.log<dbg>("Hello to ", msg->email());
	if (connectionStatusOf(source) == authed) {
		logServer.log<net>("Player `", msg->email(), "` is already connected: ",
						   connectionString(source));
		// send loginFailure response msg
		return;
	}

	using query = odb::query<db::PlayerAccount>;

	auto transaction = dbServer->begin();
	auto player = PlayerAccount_ptr{
		dbServer->query_one<db::PlayerAccount>(query::email == msg->email())};
	transaction->commit();

	if (!player) {
		logServer.log<net>("Invalid login from player `", msg->email(),
						   "` with connection: ", connectionString(source));
		// send invalidLogin message
		return;
	}

	m_connections[source] = std::move(player);

}  // end onLogin

void AuthModule::onRegistration(const msg::Registration* msg,
								WSConnection source) {
	logServer.log<net>("Registering player from connection: ",
					   connectionString(source));

	if (connectionStatusOf(source) != unauthed) {
		logServer.log<net>(
			"Registration attempt from an already registered User : ",
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
			logServer.log<net>("Invalid registration attempt; player `",
							   msg->email(),
							   "` already exists. From connection: ",
							   connectionString(source));
			// send invalid registration msg
			return;
		}
	}

	auto newPlayer =
		std::make_unique<db::PlayerAccount>(msg->email(), msg->password());

	odb::transaction insertTransaction(dbServer->begin());
	dbServer->persist(*newPlayer);
	insertTransaction.commit();

	m_connections[source] = std::move(newPlayer);
	logServer.log<net>("Player `", msg->email(), " successfully registered");
}  // end onRegistration


void AuthModule::onDisconnect(const msg::Disconnect* msg, WSConnection connection) {
	if(connectionStatusOf(connection) == authed) {
		logServer.log<net>("Disconnect; releasing auth for connection ", Utility::connectionString(connection));
		m_connections.erase(connection);
	}
} // end onDisconnect

void AuthModule::onConnect(const msg::Connect* msg, WSConnection newConnection) {

} // end onConnect
