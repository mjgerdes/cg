

#include "GameServer.hpp"
#include "GameServer/GameServerImpl.hpp"

GameServer::GameServer(WSServer& server, LogServer& logServer)
	: m_pimpl(std::make_unique<GameServer::Impl>(server, logServer)) {}
GameServer::~GameServer() {}

void GameServer::start() { m_pimpl->listen(); }

void GameServer::forwardModule(std::unique_ptr<StandardModule> module) {
	m_pimpl->loadModule(std::move(module));
}

GameServer::Database_type* GameServer::getDB() {
	return &(*(m_pimpl->dbServer));
}

GameServer::LogServer_type* GameServer::getLogServer() {
	return &(m_pimpl->logServer);
}
