

#ifndef __CARDPROVIDER_HPP__
#define __CARDPROVIDER_HPP__

#include <string>
#include <boost/container/flat_set.hpp>

#include "google/protobuf/text_format.h"
#include "CardData.pb.h"
using data_type = data::CardData;

class CardProvider {
	using data_type = data::CardData;
	struct CardDataLess {
		inline bool operator()(const data_type& a, const data_type& b) const {
			return a.id() < b.id();
		}
	};

	using container_type = boost::container::flat_set<data_type, CardDataLess>;

public:
	using CardId = unsigned int;
	class Card {};  // end class card

public:
	CardProvider(const std::string& dataPath);

	CardProvider() = delete;
	CardProvider(const CardProvider&) = delete;
	CardProvider(CardProvider&&) = delete;
	void operator=(const CardProvider&) = delete;

	const Card* get(const CardId id);
	const Card& getWithDefault(const CardId id);
	Card getCopy(const CardId id);

private:
	void init(const std::string& path);

	container_type m_cards;
};  // end class CardProvider

#endif
