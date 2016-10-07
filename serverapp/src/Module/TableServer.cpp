
#include "Module/TableServer.hpp"
#include "Module/PlayModule.hpp"

TableServer::TableServer(PlayModule& parent) : m_parent(parent) {}

void TableServer::enqueueMatch(const GameServer::ConnectionId p1,
				  const GameServer::ConnectionId p2) {
	m_incomingMatches.emplace(p1, p2);
}  // end enqueueMatch

void TableServer::run() {
}
TableServer::~TableServer() {}
