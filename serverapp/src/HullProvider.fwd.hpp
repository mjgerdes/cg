

#ifndef __HULLPROVIDER_FWD_HPP__
#define __HULLPROVIDER_FWD_HPP__

#include "Hull.hpp"
#include "ProtobufMessageLoader.hpp"
#include "ConstantDataProvider.fwd.hpp"

using HullProvider = ConstantDataProvider<
	data::HullData, Hull,
	ProtobufMessageCompileLoader<data::HullData, data::HullDataWrapper>,
	Hull::HullVerifier>;

#endif
