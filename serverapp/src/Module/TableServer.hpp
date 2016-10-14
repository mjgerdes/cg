

#ifndef __TABLESERVER_HPP__
#define __TABLESERVER_HPP__

#include "include/Concurrent/Queue.hpp"
#include "SlotPool.hpp"
#include "RandomProvider.hpp"


class Table;
class PlayModule;
class DataModule;
class AuthModule;


class TableServer {
public:
	using Table_type = Table;
	using TablePool = SlotPool<Table_type>;

public:
	TableServer(PlayModule& parent, AuthModule* auth, DataModule* data,
				GameServer::Database_type* db, GameServer::LogServer_type& ls);

	void enqueueMatch(WSConnection p1, WSConnection p2);
	void run();

	~TableServer();

private:
	void createTable(const WSConnection p1, const WSConnection p2);
	bool fillModel(Table_type* table, const WSConnection p1Connection, const WSConnection p2Connection);
	void destroyTable(TablePool::index_type i);

	void serveTables();

private:
	PlayModule& m_parent;
		AuthModule* m_auth;
	DataModule* m_data;
	GameServer::Database_type* m_db;
	GameServer::LogServer_type& logServer;
	std::unique_ptr<RandomProvider> m_rand;


	using QueueMatch_type = std::pair<WSConnection, WSConnection>;
	Concurrent::Queue<QueueMatch_type> m_incomingMatches;

};  // end class TableServer

#endif
