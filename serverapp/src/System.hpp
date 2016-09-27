

#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include "SystemData.pb.h"
#include "Card.hpp"

class System {
public:
	using Id_type = data::SystemData::SystemId;
	using data_type = data::SystemData;
	using SMS_type = Card::SMS_type;
	using Tag_type = Card::Tag_type;
	using System_ptr = std::unique_ptr<System>;

	struct SystemVerifier {
		bool operator()(const data_type* sys);

	};  // end struct SystemVerifier
public:
	System(const data_type*);

	System() = delete;
	System(const System&) = default;
	System(System&& other) = default;
	System& operator=(const System&) = default;
	System& operator=(System&& other) = default;

public:
	inline Id_type id() const { return m_data->id(); }
	inline unsigned int capacity() const { return m_data->capacity(); }
	inline unsigned int size() const { return m_cards.size(); }
	inline SMS_type type() const { return m_data->type(); }
	inline const std::vector<Card>& cards() const { return m_cards; }

	bool tryAddCards(const std::vector<Card>&);
	bool tryAddCard(const Card&);
	bool verifyCard(const Card&) const;

private:
	const data_type* m_data;
	std::vector<Card> m_cards;
};  // end class system
#endif
