

#include "ShipPlan.hpp"
#include "HullProvider.hpp"
#include "SystemProvider.hpp"

using namespace db;

ShipPlan::ShipPlan(const Hull& hull) : m_hullId(hull.id()), m_systemPlans() {
	m_systemPlans.reserve(hull.size());
	for (const auto& system : hull.systems()) {
		m_systemPlans.push_back(std::make_shared<SystemPlan>(system));
	}
}

db::ShipPlan::Hull_ptr ShipPlan::load(const HullProvider& hp,
									  const SystemProvider& sp,
									  const CardProvider& cp) {
	auto newHull = std::make_unique<Hull>(hp.get(m_hullId));
	if (!newHull) return nullptr;
	unsigned int pos = 0;
	for (const auto& systemPlan : m_systemPlans) {
		if (!newHull->trySetSystem(systemPlan->load(sp, cp), pos++)) return nullptr;
	}
	return std::move(newHull);
}
