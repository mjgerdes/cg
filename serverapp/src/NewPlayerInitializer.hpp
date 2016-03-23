

#ifndef __NEWPLAYERINITIALIZER_HPP__
#define __NEWPLAYERINITIALIZER_HPP__

#include "GameServer.hpp"
#include "db/PlayerAccount.hpp"
#include "database.hpp"
#include "db/Player.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"
#include "CardProvider.hpp"

struct NewPlayerInitializer {
	NewPlayerInitializer(GameServer::Database_type* db, CardProvider* cp)
		: m_db(db), m_cp(cp), m_initialCardGift() {
		m_initialCardGift.reserve(m_cp->size() - 1);
		for (Card::Id_type i = 1; i < m_cp->size(); ++i) {
			m_initialCardGift.push_back(i);
		}
	}

	inline void operator()(typename db::PlayerAccount::Id_type id,
						   WSConnection connection) {
		std::cout << "Init for " << id << std::endl;
		{
			odb::transaction t{m_db->begin()};
			auto newPlayer = db::Player{id};
			newPlayer.initializeCardCollection(m_initialCardGift);
			m_db->persist(newPlayer);
			t.commit();
		}
	}

private:
	GameServer::Database_type* m_db;
	CardProvider* m_cp;
	db::Player::CardContainer_type m_initialCardGift;
};  // end struct NewPlayerInitializer

#endif
