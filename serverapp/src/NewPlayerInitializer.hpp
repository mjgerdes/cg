

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
#include "db/ShipPlan.hpp"

struct NewPlayerInitializer {

/*
	NewPlayerInitializer(const NewPlayerInitializer& other)
		: m_db(other.m_db), m_cp(other.m_cp), m_sp(other.m_sp), m_hp(other.m_hp),
		  m_initialCardGift(std::move(other.m_initialCardGift)), m_initialSystemGift(std::move(other.m_initialSystemGift)),
		  m_initialHullGift(std::move(other.m_initialHullGift)),
		  m_shipPlanGift(std::move(other.m_shipPlanGift)) {}
*/	

	NewPlayerInitializer(GameServer::Database_type* db, CardProvider* cp,
						 SystemProvider* sp, HullProvider* hp)
		: m_db(db),
		  m_cp(cp),
		  m_sp(sp),
		  m_hp(hp),
		  m_initialCardGift(),
		  m_initialSystemGift(),
		  m_initialHullGift() {
		m_initialCardGift.reserve(m_cp->size() - 1);
		for (unsigned int i = data::CardData::basic_missile; i < m_cp->size();
			 ++i) {
//			m_initialCardGift.push_back(static_cast<data::CardData::CardId>(i));
			m_initialCardGift.push_back(m_cp->get(static_cast<Card::Id_type>(i)).id());
		}

		m_initialSystemGift.reserve(m_sp->size() - 1);
		for (unsigned int i = data::SystemData::missile_launcher;
			 i < m_sp->size(); ++i) {
			m_initialSystemGift.push_back(static_cast<System::Id_type>(i));
		}

		m_initialHullGift.push_back(data::HullData::basic_wombat);

		// systemPlans
		using S = data::SystemData;
		using C = data::CardData;
		auto parent = this;
		auto hull = Hull{m_hp->get(data::HullData::basic_wombat)};
		unsigned int j = 0;
		auto addSystem = [parent, &hull, &j](System::Id_type system,
								  std::initializer_list<Card::Id_type> cards) {
			auto sys = parent->m_sp->get(system);
			for (const auto& card : cards) {
				sys.tryAddCard(parent->m_cp->get(card));
			}
			hull.trySetSystem(sys, j++);
		};

		addSystem(
			S::engineering_bay,
			{C::basic_metal_plate, C::basic_metal_plate, C::basic_metal_plate,
			 C::basic_janitor, C::basic_janitor, C::basic_janitor,
			 C::basic_flare, C::basic_flare, C::basic_flare, C::basic_red});

		addSystem(S::hangar_bay,
				  {C::basic_signal_beacon, C::basic_signal_beacon,
				   C::basic_signal_beacon, C::basic_repair_droid,
				   C::basic_repair_droid, C::basic_repair_droid,
				   C::basic_hangar_crew, C::basic_hangar_crew,
				   C::basic_hangar_crew, C::basic_well_stocked});

		addSystem(
			S::missile_launcher,
			{C::basic_olga, C::basic_olga, C::basic_olga,
			 C::basic_target_vector, C::basic_target_vector,
			 C::basic_target_vector, C::basic_big_missile, C::basic_big_missile,
			 C::basic_big_missile, C::basic_revolver_missile});

		addSystem(S::computer_core,
				  {C::basic_disassemble, C::basic_disassemble,
				   C::basic_disassemble, C::basic_stackoverflow,
				   C::basic_stackoverflow, C::basic_stackoverflow, C::basic_emp,
				   C::basic_emp, C::basic_emp, C::basic_jolt_cola});

		addSystem(S::particle_lab,
				  {C::basic_lab_assistant, C::basic_lab_assistant,
				   C::basic_lab_assistant, C::basic_programmer,
				   C::basic_programmer, C::basic_programmer, C::basic_physicist,
				   C::basic_physicist, C::basic_irradiate, C::basic_irradiate});

		addSystem(
			S::laser_battery,
			{C::basic_space_marine, C::basic_space_marine,
			 C::basic_space_marine, C::basic_mercenary, C::basic_mercenary,
			 C::basic_mercenary, C::basic_laser_flurry, C::basic_laser_flurry,
			 C::basic_laser_flurry, C::basic_missile});

		m_shipPlanGift = std::make_shared<db::ShipPlan>(hull);
	}  // end constructor

	inline void operator()(typename db::PlayerAccount::Id_type id,
						   WSConnection connection) {
		std::cout << "Init for " << id << std::endl;
		{
			odb::transaction t{m_db->begin()};
			auto newPlayer = db::Player{id};
			newPlayer.initializeCardCollection(m_initialCardGift);
			newPlayer.initializeSystemCollection(m_initialSystemGift);
			newPlayer.initializeHullCollection(m_initialHullGift);

// first we persist the systems
			for(const auto& systemPlan : m_shipPlanGift->systemPlans()) {
				m_db->persist(*systemPlan);
				newPlayer.addSystemPlan(systemPlan);
			}

// now we give the ship plan, which is linked to the systems
			m_db->persist(*m_shipPlanGift);
			newPlayer.addShipPlan(m_shipPlanGift);

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
	db::Player::HullContainer_type m_initialHullGift;

// shared because std::function must be copyiable
	std::shared_ptr<db::ShipPlan> m_shipPlanGift;
};  // end struct NewPlayerInitializer

#endif
