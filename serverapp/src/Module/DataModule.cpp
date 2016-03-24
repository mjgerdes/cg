
#include "Module/DataModule.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"

DataModule::DataModule(AuthModule* auth, GameServer::Database_type* db,
					   const std::string& cardPath)
	: m_db(db), m_auth(auth), m_cp(cardPath) {}

void DataModule::bindHandlersImp(MessageDispatcher_type* dispatcher) {
	using namespace msg;
	auto reg = dispatcher->getRegisterFunction(this);
	reg.entry<CardCollectionRequest>(ClientMessage::CardCollectionRequestType,
									 &ClientMessage::card_collection_request,
									 &DataModule::onCardCollectionRequest);
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
		t.commit();
	}
}  // end onCardCollectionRequest

void DataModule::sendCardCollectionResponse(
	const db::Player::CardContainer_type& cards, WSConnection destination) {
	auto msg = makeServerMessage();
	msg->set_msgtype(msg::ServerMessage::CardCollectionResponseType);
	for (const auto& id : cards) {
		msg->mutable_card_collection_response()->add_card_ids(id);
	}
	sendMessage(msg, destination);
}  // end sendCardCollectionResponse

