

#ifndef __NEWPLAYERINITIALIZER_HPP__
#define __NEWPLAYERINITIALIZER_HPP__

#include "GameServer.hpp"
#include "db/PlayerAccount.hpp"
#include "database.hpp"
#include "db/Player.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"


struct NewPlayerInitializer {
	NewPlayerInitializer(GameServer::Database_type* db) : m_db(db) {}

	inline void operator()(typename db::PlayerAccount::Id_type id,
						   WSConnection connection) {
		std::cout << "Init for " << id << std::endl;
		{
			odb::transaction t{m_db->begin()};
			auto newPlayer = db::Player{id};
			
			m_db->persist(newPlayer);
			t.commit();
		}
	}

private:
	GameServer::Database_type* m_db;
};  // end struct NewPlayerInitializer

#endif
