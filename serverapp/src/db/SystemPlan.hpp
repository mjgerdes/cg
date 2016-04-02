

#ifndef __SYSTEMPLAN_HPP__
#define __SYSTEMPLAN_HPP__
#include <odb/core.hxx>
#include <vector>
#include <memory>
#include "Card.hpp"
#include "System.hpp"
#include "SystemProvider.fwd.hpp"
#include "CardProvider.fwd.hpp"

namespace db {

#pragma db object pointer(std::shared_ptr)
class SystemPlan {
public:
	using Id_type = unsigned long;
	using System_ptr = std::unique_ptr<System>;

public:
	SystemPlan(const System&);

	System_ptr load(const SystemProvider&, const CardProvider&);

	inline Id_type id() const { return m_id; }
	inline System::Id_type system() const { return m_systemId; }
	inline const std::vector<Card::Id_type>& cards() const { return m_cards; }

private:
	SystemPlan();

	void fillCards(const System&);

private:
	friend class odb::access;

#pragma db id auto
	Id_type m_id;
	System::Id_type m_systemId;
	std::vector<Card::Id_type> m_cards;
};  // end class SystemPlan

}  // end namespace db

#endif
