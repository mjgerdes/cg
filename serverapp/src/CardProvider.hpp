

#ifndef __CARDPROVIDER_HPP__
#define __CARDPROVIDER_HPP__

#include "CardData.pb.h"
#include "ConstantDataProvider.hpp"
#include "Card.hpp"
#include "ProtobufMessageLoader.hpp"

using CardProvider = ConstantDataProvider<data::CardData, Card, ProtobufMessageCompileLoader<data::CardData, data::CardDataWrapper>, Card::CardVerifier>;

#endif
