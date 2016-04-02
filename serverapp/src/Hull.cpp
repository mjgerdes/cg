

#include <vector>
#include <iostream>
#include "Hull.hpp"
#include "SystemData.pb.h"
#include "CardData.pb.h"

bool Hull::HullVerifier::operator()(data_type* hull) {
	auto warn = [&hull](std::string msg) {
		std::cerr << "Warning: " << hull->name() << "(" << hull->id()
				  << "): " << msg << std::endl;
	};
	if (hull->id() == data::HullData::universal) return true;

	if (hull->systems_type_size() == 0) {
		warn(
			"No system types specified. Cannot determine system count. Fatal "
			"Error!");
		return false;
	}

	if (!hull->has_name() || hull->name() == "") warn("No name given");

	if (!hull->has_description()) warn("No description set");

	if (!hull->has_fluff()) warn("No fluff text set. What is wrong with you?");

	// auto set empty systems for hull if they were not specified
	if (hull->systems_size() == 0) {
		for (const auto& type : hull->systems_type()) {
			using namespace data;
			auto sys = SystemData::empty_support;
			switch (type) {
				case CardData::science:
					sys = SystemData::empty_science;
				case CardData::military:
					sys = SystemData::empty_military;
				case CardData::support:
					sys = SystemData::empty_support;
			}  // switch
			hull->add_systems(sys);
		}  // for
	}	  // if

	if (hull->systems_size() != hull->systems_type_size()) {
		warn("Different count on systems and system types. Fatal Error");
		return false;
	}

	return true;
}  // end verifier operator()}

Hull::Hull(const data_type* hullData) : m_data(hullData), m_systems() {
m_systems.reserve(m_data->systems_type_size());
}

bool Hull::trySetSystem(System::System_ptr system, size_t pos) {
	if (!system) return false;
	return trySetSystem(*system, pos);
}

bool Hull::trySetSystem(const System& system, size_t position) {
	if (!verifySystemForPos(system, position)) return false;

	m_systems.push_back(system);
	return true;
}

bool Hull::verifySystemForPos(const System& system, size_t position) const {
	// do we even have a slot there?
	if (position >= m_data->systems_type_size()) return false;
	// is it the right type for the slot
	if (m_data->systems_type(position) != system.type()) return false;
	return true;
}

bool Hull::trySetSystemsFromSequence(const std::vector<System>& systems) {
	for (unsigned int i = 0; i < systems.size(); ++i) {
		if (!trySetSystem(systems[i], i)) return false;
	}
	return true;
}  // end trySetSystemsFromSequence
