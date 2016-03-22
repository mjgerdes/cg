
#include "Card.hpp"

bool Card::CardVerifier::operator()(const data_type* data) {
	return true;
}

Card::Card(const data_type* cardData)
	: m_data(cardData) {}
