

#include "ShipPlan.hpp"

using namespace db;

ShipPlan::ShipPlan(const Hull& hull)
	: m_hullId(hull.id()), m_systemPlans() {
	for(const auto& system : hull.systems()) {
		m_systemPlans.push_back(std::make_shared<SystemPlan>(system));
	}
}

db::ShipPlan::Hull_ptr ShipPlan::load(const HullProvider& hp, const SystemProvider& sp, const CardProvider& cp) {
	return nullptr;
}
