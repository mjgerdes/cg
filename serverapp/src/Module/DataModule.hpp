

#ifndef __DATAMODULE_HHPP__
#define __DATAMODULE_HHPP__

#include "GameServer.hpp"
#include "Module/AuthModule.hpp"
#include "CardProvider.hpp"
#include "SystemProvider.hpp"
#include "DataModuleMessages.pb.h"
#include "db/Player.hpp"

class DataModule : public GameServer::StandardModule {
public:
	DataModule(AuthModule* auth, GameServer::Database_type* db,
			   const std::string& cardPath,
			   const std::string systemPath);

	inline CardProvider* getCardProvider() { return &m_cp; };
	inline SystemProvider* getSystemProvider() { return &m_sp; }

private:
	void onCardCollectionRequest(const msg::CardCollectionRequest* msg,
								 WSConnection source);
void sendCardCollectionResponse(const db::Player::CardContainer_type& cards, WSConnection destination);
	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

private:
	GameServer::Database_type* m_db;
	AuthModule* m_auth;
	CardProvider m_cp;
	SystemProvider m_sp;
};  // end class DataModule

#endif
