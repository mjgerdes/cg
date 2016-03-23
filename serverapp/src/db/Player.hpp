

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include <odb/core.hxx>
#include "PlayerAccount.hpp"
#include <odb/callback.hxx>
#include <memory>
#include <vector>
#include <odb/database.hxx>


namespace db {

#pragma db object callback(init)
class Player {
public:
	using Id_type = typename PlayerAccount::Id_type;

	Player(Id_type newId) : m_id(newId) {}

private:
	friend class odb::access;

	Player() {}

#pragma db id
	Id_type m_id;

#pragma db transient
	std::unique_ptr<PlayerAccount> m_account;
	//		std::vector<Card::CardId_type> m_cardCollection;

	void init(odb::callback_event e, odb::database& db); 

};  // end class Player
}  // end namespace db

#endif
