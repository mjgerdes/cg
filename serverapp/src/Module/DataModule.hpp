

#ifndef __DATAMODULE_HHPP__
#define __DATAMODULE_HHPP__

#include "GameServer.hpp"
#include "Module/AuthModule.hpp"
#include "CardProvider.hpp"
#include "SystemProvider.hpp"
#include "HullProvider.hpp"
#include "DataModuleMessages.pb.h"
#include "db/Player.hpp"

class DataModule : public GameServer::StandardModule {
public:
	DataModule(AuthModule* auth, GameServer::Database_type* db,
			   const std::string& cardPath, const std::string systemPath,
			   const std::string hullPath);

	inline CardProvider* getCardProvider() { return &m_cp; };
	inline SystemProvider* getSystemProvider() { return &m_sp; }
	inline HullProvider* getHullProvider() { return &m_hp;}

private:
	void onCardCollectionRequest(const msg::CardCollectionRequest* msg,
								 WSConnection source);
	void onSystemCollectionRequest(const msg::SystemCollectionRequest* msg,
								   WSConnection source);
	void sendCardCollectionResponse(const db::Player::CardContainer_type& cards,
									WSConnection destination);
	void sendSystemCollectionResponse(
		const db::Player::SystemContainer_type& systems,
		WSConnection destination);
	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

private:
	GameServer::Database_type* m_db;
	AuthModule* m_auth;
	CardProvider m_cp;
	SystemProvider m_sp;
	HullProvider m_hp;
};  // end class DataModule

#endif
