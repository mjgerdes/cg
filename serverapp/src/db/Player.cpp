

#include "db/Player.hpp"
#include "PlayerAccount_odb.h"
#include "Player_odb.h"
namespace db {

void Player::init(odb::callback_event e, odb::database& db) {
	if (e == odb::callback_event::post_load) {
		m_account = std::move(
			std::unique_ptr<PlayerAccount>{db.load<PlayerAccount>(m_id)});
	}
}  // end init

void Player::initializeCardCollection(const CardContainer_type& initial) {
	m_cardCollection = initial;
}

void Player::initializeSystemCollection(const SystemContainer_type& systems) {
	m_systemCollection = systems;
}

void Player::initializeHullCollection(const HullContainer_type& hullGift) {
	m_hullCollection = hullGift;
}
}  // end namespace
