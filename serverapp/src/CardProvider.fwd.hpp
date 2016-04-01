

#ifndef __CARDPROVIDER_FWD_HPP__
#define __CARDPROVIDER_FWD_HPP__

#include "ConstantDataProvider.fwd.hpp"
#include "CardData.pb.h"
#include "Card.hpp"
#include "ProtobufMessageLoader.hpp"

using CardProvider = ConstantDataProvider<
	data::CardData, Card,
	ProtobufMessageCompileLoader<data::CardData, data::CardDataWrapper>,
	Card::CardVerifier>;
#endif
