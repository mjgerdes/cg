
#include <iostream>
#include "System.hpp"
#include "Card.hpp"

System::System(const data_type* sys) : m_data(sys) {}

bool System::SystemVerifier::operator()(const data_type* sys) {
	auto warn = [&sys](std::string warning) {
		std::cerr << "  Warning: " << sys->name() << "(" << sys->id()
				  << "): " << warning << std::endl;
	};



	// id
	if (sys->id() == data_type::universal ||
		sys->id() == data_type::empty_science ||
		sys->id() == data_type::empty_military ||
		sys->id() == data_type::empty_support)
		return true;


// name
	if (!sys->has_name()) {
		std::cerr << "Fatal erro: System with id " << sys->id()
				  << " has no name!\n";
		return false;
	}


	// description set
	if (!sys->has_description() || sys->description().empty())
		warn("No description set");

	if (!sys->has_capacity()) warn("No capacity set");

	// smstype
	if (!sys->has_type() || sys->type() == data::CardData::none)
		warn("No type set");

	if (!sys->has_health()) warn("No health set");

	// proc
	if (!sys->has_proc()) warn("No proc set");

	if (!sys->has_health()) warn("No health set");

	if (!sys->has_fluff() || sys->fluff() == "")
		warn("No fluff text set! Shame on you!");

	return true;
}  // end operator() of Verifier

bool System::tryAddCard(const Card& card) {
	if (size() >= capacity()) return false;

	if (!verifyCard(card)) return false;

	m_cards.push_back(card);
	return true;
}  // end tryAddCard

bool System::verifyCard(const Card& card) const {
	if (card.type() != type()) return false;

	if (card.tagCount() != 0) {
		for(unsigned int i = 0; i < m_data->card_tags_size(); ++i) {
			if(card.hasTag(m_data->card_tags(i)))
				return true;
		}
		return false;
	}
	return true;
} // end verify

bool System::tryAddCards(const std::vector<Card>& cards) {
	for(const auto& card : cards) {
		if(!tryAddCard(card))
			return false;
	}
	return true;
}






