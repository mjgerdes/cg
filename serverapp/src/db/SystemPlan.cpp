

#include "SystemPlan.hpp"
#include "SystemProvider.hpp"
#include "CardProvider.hpp"

using namespace db;


SystemPlan::SystemPlan()
	: m_systemId(data::SystemData::universal), m_cards() {}

SystemPlan::SystemPlan(const System& system)
	: m_systemId(system.id()),
	  m_cards() {
	fillCards(system);
}
void SystemPlan::fillCards(const System& system) {
	m_cards.resize(system.size());
	std::transform(system.cards().cbegin(), system.cards().cend(),
				   m_cards.begin(), [](const Card& card) { return card.id(); });
}

db::SystemPlan::System_ptr SystemPlan::load(const SystemProvider& sp, const CardProvider& cp) {
	auto system = std::make_unique<System>(sp.get(m_systemId));
	for (const auto& cardId : m_cards) {
		if (!system->tryAddCard(cp.get(cardId))) return nullptr;
	}
	return std::move(system);
}  // end load
