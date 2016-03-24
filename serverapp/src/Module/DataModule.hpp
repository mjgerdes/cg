

#ifndef __DATAMODULE_HHPP__
#define __DATAMODULE_HHPP__

#include "GameServer.hpp"
#include "Module/AuthModule.hpp"
#include "CardProvider.hpp"
#include "DataModuleMessages.pb.h"

class DataModule : public GameServer::StandardModule {
public:
	DataModule(AuthModule* auth, GameServer::Database_type* db,
			   const std::string& cardPath);

	inline CardProvider* getCardProvider() { return &m_cp; };

private:
	void onCardCollectionRequest(const msg::CardCollectionRequest* msg,
								 WSConnection source);
	void sendCardCollectionResponse(WSConnection destination);
	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

private:
	GameServer::Database_type* m_db;
	AuthModule* m_auth;
	CardProvider m_cp;
};  // end class DataModule

#endif
