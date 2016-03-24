
#include "Module/DataModule.hpp"


DataModule::DataModule(AuthModule* auth, GameServer::Database_type* db, const std::string& cardPath) :m_db(db), m_auth(auth), m_cp(cardPath) {}

void DataModule::bindHandlersImp(MessageDispatcher_type* dispatcher) {

}

void DataModule::onCardCollectionRequest(const msg::CardCollectionRequest* msg, WSConnection source) {
}

void sendCardCollectionResponse(WSConnection destination) {
}
