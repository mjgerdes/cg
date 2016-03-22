

#ifndef __CARD_HPP__
#define __CARD_HPP__

#include "CardData.pb.h"

class Card {
public:
	using id_type = unsigned int;
	using data_type = data::CardData;

	struct CardVerifier {
		bool operator()(const data_type*);
	}; // end struct CardVerifier

public:
	Card(const data_type*);

	Card() = delete;
	Card(const Card&) = default;
	Card(Card&& other) = default;
	Card& operator=(const Card&) = default;
	Card& operator=(Card&& other) = default;



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
