

#include "Module/PlayModule.hpp"
#include "LogServer.hpp"
#include "NetUtility.hpp"
#include "database.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"
#include "Module/MMRQueue.hpp"
#include "Module/TableServer.hpp"

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
	  m_tableServer(std::move(std::make_unique<TableServer>(*this, m_auth, m_data, m_db, logServer))),
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
	auto maybeConnectionp1 = m_auth->reverseLookup(p1);
	auto maybeConnectionp2 = m_auth->reverseLookup(p2);

// check for disconnect that might have slipped through
	if(!maybeConnectionp1) {
// FIXME: put the p2 back into queue
		return;
	}

	if(!maybeConnectionp2) {
		// same
		return;
	}

	
	m_tableServer->enqueueMatch(*maybeConnectionp1, *maybeConnectionp2);
}  // end startPlayModeFor

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
