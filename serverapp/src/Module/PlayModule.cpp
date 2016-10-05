

#include "Module/PlayModule.hpp"
#include "LogServer.hpp"
#include "NetUtility.hpp"
#include "database.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"

using namespace Log;
using namespace Utility;

struct PlayMessage : LogServer::LogMessage {
	PlayMessage(const std::string& msg) : LogServer::LogMessage(msg) {}
	inline virtual void write() override {
		std::cout << "[PLAY] " << m_msg << std::endl;
	}
};  // end struct Authmessage

using play = PlayMessage;

PlayModule::PlayModule(AuthModule* auth, DataModule* data,
					   GameServer::Database_type* db,
					   GameServer::LogServer_type* ls)
	: m_auth(auth), m_data(data), m_db(db), logServer(*ls) {}

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
		//		t.commit();
	}

}  // end onPlayInitiationRequest
