

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include <odb/core.hxx>
#include "PlayerAccount.hpp"
#include <odb/callback.hxx>
#include <memory>
#include <vector>
#include <odb/database.hxx>
#include "Card.hpp"
#include "System.hpp"
#include "Hull.hpp"
#include "ShipPlan.hpp"

namespace db {

#pragma db object callback(init)
class Player {
public:
	using Id_type = typename PlayerAccount::Id_type;
	using CardContainer_type = std::vector<Card::Id_type>;
	using SystemContainer_type = std::vector<System::Id_type>;
	using HullContainer_type = std::vector<Hull::Id_type>;

public:
	Player(Id_type newId) : m_id(newId) {}

	inline const PlayerAccount& account() const { return *m_account; }
	void initializeCardCollection(const CardContainer_type&);
	void initializeSystemCollection(const SystemContainer_type&);
	void initializeHullCollection(const HullContainer_type&);
	inline const CardContainer_type& getCardCollection() const {
		return m_cardCollection;
	}

	inline const SystemContainer_type& getSystemCollection() {
		return m_systemCollection;
	}

	inline const HullContainer_type& getHullCollection() {
		return m_hullCollection;
	}

	inline void addShipPlan(std::shared_ptr<ShipPlan> plan) {
		m_shipPlanCollection.push_back(plan);
	}

	inline void addSystemPlan(std::shared_ptr<SystemPlan> plan) {
		m_systemPlanCollection.push_back(plan);
	}

private:
	friend class odb::access;

	Player() {}

private:
#pragma db id
	Id_type m_id;

#pragma db transient
	std::unique_ptr<PlayerAccount> m_account;

#pragma db unordered
	CardContainer_type m_cardCollection;
#pragma db unordered
	SystemContainer_type m_systemCollection;
#pragma db unordered
	HullContainer_type m_hullCollection;

#pragma db value_not_null
	std::vector<std::shared_ptr<db::SystemPlan>> m_systemPlanCollection;
#pragma db value_not_null
	std::vector<std::shared_ptr<ShipPlan>> m_shipPlanCollection;

	void init(odb::callback_event e, odb::database& db);

};  // end class Player
}  // end namespace db

#endif
