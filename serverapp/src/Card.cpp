
#include <iostream>
#include <sstream>
#include <string>
#include "Card.hpp"

bool Card::CardVerifier::operator()(const data_type* data) {
	std::stringstream namestringstream;
	namestringstream << " (card id: " << data->id()
					 << ", name: " << data->name() << ")\n";
	std::string warningstring{"Warning in Card::verifier: "};
	std::string namestring{namestringstream.str()};

	if (data->id() == data_type::universal) return true;

	if (data->type() == data_type::none)
		std::cerr << warningstring << "No SMS type set " << namestring;

	if (data->name().empty())
		std::cerr << warningstring << "No name set " << namestring;

	if (data->description().empty())
		std::cerr << warningstring << "No description set" << namestring;

	if (data->fluff().empty())
		std::cerr << warningstring << "No fluff text set, shame on you!"
				  << namestring;

	if (data->system_id() == 0)
		std::cerr << warningstring << "No system specified" << namestring;

	return true;
}

Card::Card(const data_type* cardData) : m_data(cardData) {}
