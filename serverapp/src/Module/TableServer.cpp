#include "Table.hpp"
#include "Module/TableServer.hpp"
#include "Module/PlayModule.hpp"
#include "Module/AuthModule.hpp"
#include "Module/DataModule.hpp"

#include "database.hpp"
#include "db/Player.hpp"
#include "Player_odb.h"
#include "CardProvider.hpp"
#include "SystemProvider.hpp"
#include "HullProvider.hpp"
#include "db/ShipPlan.hpp"

// constructor
TableServer::TableServer(PlayModule& parent, AuthModule* auth, DataModule* data,
						 GameServer::Database_type* db,
						 GameServer::LogServer_type& ls)
	: m_parent(parent),
	  m_auth(auth),
	  m_data(data),
	  m_db(db),
	  logServer(ls),
	  m_rand(std::move(std::make_unique<RandomProvider>())) {
	Table_type::setDataProvider(*m_data);
}

void TableServer::enqueueMatch(WSConnection p1, WSConnection p2) {
	m_incomingMatches.emplace(p1, p2);
}  // end enqueueMatch

void TableServer::run() {
	while (true) {
		// check if we have to create a new table
		if (!m_incomingMatches.empty()) {
			auto ps = m_incomingMatches.pop();
			createTable(ps.first, ps.second);
		}

		// check if anything needs to be done for the tables running
		serveTables();
	}  // end while
}  // end run

void TableServer::createTable(const WSConnection p1, const WSConnection p2) {
	auto i = TablePool::create();
	auto table = TablePool::at(i);
	table.initialize(i);
	// fill in cards from shipplan
	if (!fillModel(&table, p1, p2)) {
		return;  // arghhhg!!11
	}

	// FIXME:  send messages to players
}

bool TableServer::fillModel(Table_type* table, const WSConnection p1Connection,
							const WSConnection p2Connection) {
	auto p1MaybeId = m_auth->getIdFor(p1Connection);
	auto p2MaybeId = m_auth->getIdFor(p1Connection);
	if (!p1MaybeId || !p2MaybeId) {
		// Player started a game without being registered
		// FIXME: how did we get here?
		return false;
	}

	db::Player::ShipPlanCollection_type p1ShipPlan;
	db::Player::ShipPlanCollection_type p2ShipPlan;
	{
		odb::transaction t{m_db->begin()};
		auto player1 =
			std::unique_ptr<db::Player>{m_db->load<db::Player>(*p1MaybeId)};
		auto player2 =
			std::unique_ptr<db::Player>{m_db->load<db::Player>(*p2MaybeId)};
		p1ShipPlan = player1->getShipPlans();
		p2ShipPlan = player2->getShipPlans();
	}  // end db

	// FIXME: we always just take the first shipplan (we know its there), only
	// for testing, add lookup of prefered plan of player here
	// hulls
	const auto p1Ship = p1ShipPlan[0];
	const auto p2Ship = p2ShipPlan[0];
	table->m_model.set_player1_hull_id(p1Ship->hull());
	table->m_model.set_player2_hull_id(p2Ship->hull());

	// systems

	const auto p1Systems = p1Ship->systemPlans();
	const auto p2Systems = p2Ship->systemPlans();

	// we need some unique random numbers for both players
	auto r = m_rand->getUniqueSequenceGenerator();

	// shorthand for convenience
	auto model = table->m_model;

	// add systems, fill in systems with cards, obfuscate the cards randomly and
	// memorize the obfuscation
	for (const auto& p1sys : p1Systems) {
		model.add_player1_system_ids(p1sys->system());

		// pointer to the new container of the newly added systems's cards
		// (still empty)
		auto sysCards = model.add_player1_system_cards();
		// need to get the cards to add to this system and shuffle them,
		// unfortunately we hav to copy here
		auto newCards = p1sys->cards();
		m_rand->shuffle(&newCards);

		// add them one by one to the sysCards container
		for (const auto cardId : newCards) {
			auto obfuscatedCardId = r();
			table->associate(obfuscatedCardId, cardId);
			sysCards->add_obfuscated_card_id(obfuscatedCardId);
		}
} // end for

	for (const auto& p2sys : p1Systems) {
		model.add_player2_system_ids(p2sys->system());

		// pointer to the new container of the newly added systems's cards
		// (still empty)
		auto sysCards = model.add_player2_system_cards();
		// need to get the cards to add to this system and shuffle them,
		// unfortunately we hav to copy here
		auto newCards = p2sys->cards();
		m_rand->shuffle(&newCards);

		// add them one by one to the sysCards container
		for (const auto cardId : newCards) {
			auto obfuscatedCardId = r();
			table->associate(obfuscatedCardId, cardId);
			sysCards->add_obfuscated_card_id(obfuscatedCardId);
		}
} // end for




	return true;
}  // end fillModel

void TableServer::destroyTable(TablePool::index_type i) {
	// FIXME:  send players table destroyed messag
	TablePool::erase(i);
}

void TableServer::serveTables() {}  // end serveTables

TableServer::~TableServer() {}
