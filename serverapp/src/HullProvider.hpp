

#ifndef __HULLPROVIDER_HPP__
#define __HULLPROVIDER_HPP__

#include "Hull.hpp"
#include "ProtobufMessageLoader.hpp"

using HullProvider = ConstantDataProvider<
	data::HullData, System,
	ProtobufMessageCompileLoader<data::HullData, data::HullDataWrapper>,
	Hull::HullVerifier>;

#endif
