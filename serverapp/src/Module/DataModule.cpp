
#include "Module/DataModule.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"

DataModule::DataModule(AuthModule* auth, GameServer::Database_type* db,
					   const std::string& cardPath,
					   const std::string systemPath, const std::string hullPath)
	: m_db(db),
	  m_auth(auth),
	  m_cp(cardPath),
	  m_sp(systemPath),
	  m_hp(hullPath) {}

void DataModule::bindHandlersImp(MessageDispatcher_type* dispatcher) {
	using namespace msg;
	auto reg = dispatcher->getRegisterFunction(this);
	reg.entry<CardCollectionRequest>(ClientMessage::CardCollectionRequestType,
									 &ClientMessage::card_collection_request,
									 &DataModule::onCardCollectionRequest);
	reg.entry<SystemCollectionRequest>(
		ClientMessage::SystemCollectionRequestType,
		&ClientMessage::system_collection_request,
		&DataModule::onSystemCollectionRequest);
}

void DataModule::onCardCollectionRequest(const msg::CardCollectionRequest* msg,
										 WSConnection source) {
	auto maybeId = m_auth->getIdFor(source);
	if (!maybeId) {
		return;
	}

	{
		odb::transaction t(m_db->begin());
		auto player =
			std::unique_ptr<db::Player>{m_db->load<db::Player>(*maybeId)};
		sendCardCollectionResponse(player->getCardCollection(),
								   std::move(source));
		//		t.commit();
	}
}  // end onCardCollectionRequest

void DataModule::onSystemCollectionRequest(
	const msg::SystemCollectionRequest* msg, WSConnection source) {
	auto maybeId = m_auth->getIdFor(source);
	if (!maybeId) {
		return;
	}

	{
		odb::transaction t(m_db->begin());
		auto player =
			std::unique_ptr<db::Player>{m_db->load<db::Player>(*maybeId)};
		sendSystemCollectionResponse(player->getSystemCollection(),
									 std::move(source));
	}
}  // end onSystemCollectionRequest

void DataModule::sendCardCollectionResponse(
	const db::Player::CardContainer_type& cards, WSConnection destination) {
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::CardCollectionResponseType);
	for (const auto& id : cards) {
		msg->mutable_card_collection_response()->add_card_ids(id);
	}
	sendMessage(msg, destination);
}  // end sendCardCollectionResponse

void DataModule::sendSystemCollectionResponse(
	const db::Player::SystemContainer_type& systems, WSConnection destination) {
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::SystemCollectionResponseType);
	for (const auto& id : systems) {
		std::cout << id << std::endl;
		msg->mutable_system_collection_response()->add_system_ids(id);
	}
	sendMessage(msg, destination);
}  // end sendsystemCollectionResponse
