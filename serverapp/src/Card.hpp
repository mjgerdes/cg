

#ifndef __CARD_HPP__
#define __CARD_HPP__

#include "CardData.pb.h"

class Card {
public:
	using data_type = data::CardData;
	using Id_type = typename data_type::CardId;
	using SMS_type = data::CardData::SMSType;
	using Tag_type = data::CardTag::TagId;
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

public:
	inline data_type::CardId id() const { return m_data->id(); }
	inline SMS_type type() const { return m_data->type(); }
	inline unsigned int tagCount() const { return m_data->tags_size(); }
	bool hasTag(const Tag_type) const;

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
