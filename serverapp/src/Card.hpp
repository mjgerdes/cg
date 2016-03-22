

#ifndef __CARD_HPP__
#define __CARD_HPP__

#include "CardData.pb.h"

class Card {
public:
	using id_type = unsigned int;
	using data_type = data::CardData;

	Card(const data_type*);

private:
	const data_type* m_data;
};  // end class card

/*
class CardProvider::Card {
	friend class CardProvider;
	Card(const CardProvider::data_type* cardData);

private:
	const CardProvider::data_type* m_data;
}; // end class Card
*/

#endif
