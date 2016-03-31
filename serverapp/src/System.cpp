
#include <iostream>
#include "System.hpp"

System::System(const data_type* sys) : m_data(sys) {}

bool System::SystemVerifier::operator()(const data_type* sys) {
	auto warn = [&sys](std::string warning) {
		std::cerr << "  Warning: " << sys->name() << "(" << sys->id()
				  << "): " << warning << std::endl;
	};

	// name
	if (!sys->has_name()) {
		std::cerr << "Fatal erro: System with id " << sys->id()
				  << " has no name!\n";
		return false;
	}

	// id
	if (sys->id() == data_type::universal
		|| sys->id() == data_type::empty_science
		|| sys->id() == data_type::empty_military
		|| sys->id() == data_type::empty_support) return true;

	// description set
	if (!sys->has_description() || sys->description().empty())
		warn("No description set");

	if(!sys->has_capacity()) 
		warn("No capacity set");

// smstype
	if(!sys->has_type() || sys->type() == data::CardData::none)
		warn("No type set");

	if(!sys->has_health())
		warn("No health set");

	// proc
	if(!sys->has_proc())
		warn("No proc set");

	if(!sys->has_health())
		warn("No health set");

	if(!sys->has_fluff() || sys->fluff() == "")
		warn("No fluff text set! Shame on you!");

	return true;
}
