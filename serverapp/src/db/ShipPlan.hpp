

#ifndef __SHIPPLAN_HPP__
#define __SHIPPLAN_HPP__

#include <vector>
#include <memory>
#include "SystemPlan.hpp"
#include "Hull.hpp"
#include "HullProvider.fwd.hpp"
#include "SystemProvider.fwd.hpp"
#include "CardProvider.fwd.hpp"

namespace db {

#pragma db object pointer(std::shared_ptr)
class ShipPlan {
public:
	using Id_type = unsigned long;
	using Hull_ptr = std::unique_ptr<Hull>;
	using SystemPlan_ptr = std::shared_ptr<SystemPlan>;
	using SystemPlans_type = std::vector<SystemPlan_ptr>;

public:
	ShipPlan(const Hull& hull);

	inline Id_type id() const { return m_id; }
	inline Hull::Id_type hull() const { return m_hullId; }
	inline const SystemPlans_type& systemPlans() const { return m_systemPlans; }

	Hull_ptr load(const HullProvider&, const SystemProvider&, const CardProvider&);

private:
	friend class odb::access;
	ShipPlan() {}

#pragma db id auto
	Id_type m_id;
	Hull::Id_type m_hullId;
	SystemPlans_type m_systemPlans;
};  // end class ShipPlan

}  // end namespace db
#endif
