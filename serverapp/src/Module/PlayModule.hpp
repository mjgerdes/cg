

#ifndef __PLAYMODULE_HPP__
#define __PLAYMODULE_HPP__

#include "GameServer.hpp"
#include "Module/AuthModule.hpp"
#include "Module/DataModule.hpp"
#include <thread>

class MMRQueue;
class TableServer;
class Table;

class PlayModule : public GameServer::StandardModule {
public:
	PlayModule(AuthModule* auth, DataModule* data,
			   GameServer::Database_type* db, GameServer::LogServer_type* ls);

	void startPlayModeFor(const GameServer::ConnectionId p1,
						  const GameServer::ConnectionId p2);

	void sendTable(const Table& table, WSConnection player1,
				   WSConnection player2) const;

	// thanks unique_ptr
	~PlayModule();

private:
	void onPlayInitiationRequest(const msg::PlayInitiationRequest* msg,
								 WSConnection source);

	void sendTableStartMessage(WSConnection& p1, WSConnection& p2) const;
	void sendObfuscationTableMessage(const Table& table,
									 WSConnection destination) const;
	void bindHandlersImp(MessageDispatcher_type* dispatcher) override;

private:
	AuthModule* m_auth;
	DataModule* m_data;
	GameServer::Database_type* m_db;
	GameServer::LogServer_type& logServer;

	friend class MMRQueue;
	std::unique_ptr<MMRQueue> m_mmr;
	std::thread m_mmrThread;

	friend class TableServer;
	std::unique_ptr<TableServer> m_tableServer;
	std::thread m_tableThread;
};

#endif
