

#include "Module/PlayModule.hpp"
#include "LogServer.hpp"
#include "NetUtility.hpp"
#include "database.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"
#include "Module/MMRQueue.hpp"
#include "Module/TableServer.hpp"
#include "Module/Table.hpp"

using namespace Log;
using namespace Utility;

struct PlayMessage : LogServer::LogMessage {
	PlayMessage(const std::string& msg) : LogServer::LogMessage(msg) {}
	inline virtual void write() override {
		std::cout << "[PLAY] " << m_msg << std::endl;
	}
};  // end struct playmessage

using play = PlayMessage;

PlayModule::PlayModule(AuthModule* auth, DataModule* data,
					   GameServer::Database_type* db,
					   GameServer::LogServer_type* ls)
	: m_auth(auth),
	  m_data(data),
	  m_db(db),
	  logServer(*ls),
	  m_mmr(std::move(std::make_unique<MMRQueue>(*this))),
	  m_mmrThread([this]() { m_mmr->run(); }),
	  m_tableServer(std::move(std::make_unique<TableServer>(
		  *this, m_auth, m_data, m_db, logServer))),
	  m_tableThread([this]() { m_tableServer->run(); }) {
	// this constructor spawns a new thread for the matchmaking queue
	logServer.log<dbg>("Started MMR thread with id ", m_mmrThread.get_id());
	m_mmrThread.detach();
	logServer.log<dbg>("Started Table server thread with id ",
					   m_tableThread.get_id());
	m_tableThread.detach();
}  // end constructor

void PlayModule::startPlayModeFor(const GameServer::ConnectionId p1,
								  const GameServer::ConnectionId p2) {
	// this function must be thread safe; gets called from MMRQueue
	// FIXME: it's not
	logServer.log<play>("Starting a playmode...");
	auto maybeConnectionp1 = m_auth->reverseLookup(p1);
	auto maybeConnectionp2 = m_auth->reverseLookup(p2);

	// check for disconnect that might have slipped through
	if (!maybeConnectionp1) {
		// FIXME: put the p2 back into queue
		return;
	}

	if (!maybeConnectionp2) {
		// same
		return;
	}

	sendTableStartMessage(*maybeConnectionp1, *maybeConnectionp2);
	m_tableServer->enqueueMatch(*maybeConnectionp1, *maybeConnectionp2);
}  // end startPlayModeFor

void PlayModule::sendObfuscationTableMessage(const Table& table,
											 WSConnection destination) const {
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::ObfuscationTableMessageType);
	*(msg->mutable_obfuscation_table_message()) =
		table.obfuscationTableMessage();
	sendMessage(msg, destination);
}  // end sendObfuscationTableMessage

void PlayModule::sendTableStartMessage(WSConnection& p1, WSConnection& p2) const {
	auto msg1 = makeServerMessage();
	auto msg2 = makeServerMessage();
	msg1->set_msgtype(msg::ServerMessage::TableStartMessageType);
	msg2->set_msgtype(msg::ServerMessage::TableStartMessageType);

	msg1->mutable_table_start_message()->set_my_player_number(1);
	msg1->mutable_table_start_message()->set_enemy_name("");
	msg2->mutable_table_start_message()->set_my_player_number(2);
	msg2->mutable_table_start_message()->set_enemy_name("");

	sendMessage(msg1, p1);
	sendMessage(msg2, p2);
}

void PlayModule::sendTable(const Table& table, WSConnection player1,
						   WSConnection player2) const {
	logServer.log<play>("Sending out table information to (Player1:",
						connectionString(player1), ", Player2: ",
						connectionString(player2));
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::TableType);
	*(msg->mutable_table()) = table.raw();

	//first we send the obfuscation table to interpret the card ids


// actually send the table model
	sendMessage(msg, player1);
	sendMessage(msg, player2);
}  // end sendTable

void PlayModule::bindHandlersImp(MessageDispatcher_type* dispatcher) {
	using namespace msg;
	auto reg = dispatcher->getRegisterFunction(this);
	reg.entry<PlayInitiationRequest>(ClientMessage::PlayInitiationRequestType,
									 &ClientMessage::play_initiation_request,
									 &PlayModule::onPlayInitiationRequest);
}

void PlayModule::onPlayInitiationRequest(const msg::PlayInitiationRequest* msg,
										 WSConnection source) {
	auto maybeId = m_auth->getIdFor(source);
	if (!maybeId) {
		logServer.log<play>("Play initiation request from unauthed connection ",
							connectionString(source));
		// send invalid request msg
		return;
	}

	{
		odb::transaction t(m_db->begin());
		auto player =
			std::unique_ptr<db::Player>{m_db->load<db::Player>(*maybeId)};
		logServer.log<play>("Starting to queue ", player->account().email(),
							" from connection ", connectionString(source));

		// FIXME: Player might already be queueing
		// FIXME: ConnectionId is based on address in the implementation of
		// the websock server: This might get us into pointer aliasing
		// problems.
		m_mmr->enqueue(std::make_pair(&(*source), 0));
		//		t.commit();
	}
}  // end onPlayInitiationRequest

PlayModule::~PlayModule() {}
