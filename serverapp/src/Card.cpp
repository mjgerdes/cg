
#include <iostream>
#include <sstream>
#include <string>
#include "Card.hpp"

bool Card::CardVerifier::operator()(const data_type* data) {
	std::stringstream namestringstream;
	namestringstream << data->name() << "(" << data->id() << "): ";
	std::string warningstring{"  Warning: "};
	std::string namestring{namestringstream.str()};
	warningstring += namestring;

	if (data->id() == data_type::universal) return true;

	if (data->type() == data_type::none)
		std::cerr << warningstring << "No SMS type set\n";

	if (data->name().empty()) std::cerr << warningstring << "No name set\n";

	if (data->category() == 0)
		std::cerr << warningstring << "No category specified\n";

	if (!data->has_description())
		std::cerr << warningstring << "No description set\n";

	if (data->fluff().empty())
		std::cerr << warningstring << "No fluff text set, shame on you!\n";

	//	if (data->system_id() == 0)
	//		std::cerr << warningstring << "No system specified\n";

	//	if(data->tags_size() == 0)
	//		std::cerr << warningstring << "No tags set\n";

	if (data->category() == data::CardCategory::crew &&
		data->science_provide() == 0 && data->military_provide() == 0 &&
		data->support_provide() == 0)
		std::cerr << warningstring
				  << "Card is of type crew but provides nothing. Stowaway?\n";

	return true;
}

Card::Card(const data_type* cardData) : m_data(cardData) {}

bool Card::hasTag(const Tag_type tag) const {
	for (unsigned int i = 0; i < m_data->tags_size(); ++i) {
		if (tag == m_data->tags(i)) return true;
	}
	return false;
}  // end hasTag
