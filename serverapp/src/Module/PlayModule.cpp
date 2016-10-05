

#include "Module/PlayModule.hpp"

PlayModule::PlayModule(AuthModule* auth, DataModule* data,
					   GameServer::Database_type* db)
	: m_db(db), m_auth(auth), m_data(data) {}

void PlayModule::bindHandlersImp(MessageDispatcher_type* dispatcher) {
	using namespace msg;
	auto reg = dispatcher->getRegisterFunction(this);
	reg.entry<PlayInitiationRequest>(ClientMessage::PlayInitiationRequestType,
									 &ClientMessage::play_initiation_request,
									 &PlayModule::onPlayInitiationRequest);
}

void PlayModule::onPlayInitiationRequest(const msg::PlayInitiationRequest* msg,
										 WSConnection source) {}
