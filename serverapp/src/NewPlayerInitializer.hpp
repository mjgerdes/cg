

#ifndef __NEWPLAYERINITIALIZER_HPP__
#define __NEWPLAYERINITIALIZER_HPP__

#include "GameServer.hpp"
#include "db/PlayerAccount.hpp"
#include "database.hpp"
#include "db/Player.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"
#include "CardProvider.hpp"
#include "SystemProvider.hpp"
#include "HullProvider.hpp"

struct NewPlayerInitializer {
	NewPlayerInitializer(GameServer::Database_type* db, CardProvider* cp,
						 SystemProvider* sp,
		HullProvider* hp)
		: m_db(db), m_cp(cp), m_sp(sp), m_hp(hp), m_initialCardGift() {
		m_initialCardGift.reserve(m_cp->size() - 1);
		for (unsigned int i = data::CardData::basic_missile; i < m_cp->size();
			 ++i) {
			m_initialCardGift.push_back(static_cast<data::CardData::CardId>(i));
		}

		m_initialSystemGift.reserve(m_sp->size() - 1);
		for (unsigned int i = data::SystemData::missile_launcher;
			 i < m_sp->size(); ++i) {
			m_initialSystemGift.push_back(static_cast<System::Id_type>(i));
		}

	}  // end constructor

	inline void operator()(typename db::PlayerAccount::Id_type id,
						   WSConnection connection) {
		std::cout << "Init for " << id << std::endl;
		{
			odb::transaction t{m_db->begin()};
			auto newPlayer = db::Player{id};
			newPlayer.initializeCardCollection(m_initialCardGift);
			newPlayer.initializeSystemCollection(m_initialSystemGift);
			m_db->persist(newPlayer);
			t.commit();
		}
	}

private:
	GameServer::Database_type* m_db;
	CardProvider* m_cp;
	SystemProvider* m_sp;
	HullProvider* m_hp;
	db::Player::CardContainer_type m_initialCardGift;
	db::Player::SystemContainer_type m_initialSystemGift;
};  // end struct NewPlayerInitializer

#endif
