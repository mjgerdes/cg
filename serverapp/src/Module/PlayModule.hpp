

#ifndef __PLAYMODULE_HPP__
#define __PLAYMODULE_HPP__

#include "GameServer.hpp"
#include "Module/AuthModule.hpp"
#include "Module/DataModule.hpp"

class MMRQueue;

class PlayModule : public GameServer::StandardModule {
public:

	
public:
	PlayModule(AuthModule* auth, DataModule* data,
			   GameServer::Database_type* db, GameServer::LogServer_type* ls);
// thanks unique_ptr
	~PlayModule();
private:
	void onPlayInitiationRequest(const msg::PlayInitiationRequest* msg,
								 WSConnection source);

	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

private:
	AuthModule* m_auth;
	DataModule* m_data;
	GameServer::Database_type* m_db;
	GameServer::LogServer_type& logServer;

	std::unique_ptr<MMRQueue> m_mmr;
};

#endif
