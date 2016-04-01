

#ifndef __SYSTEMPROVIDER_FWD_HPP__
#define __SYSTEMPROVIDER_FWD_HPP__

#include "ConstantDataProvider.fwd.hpp"
#include "System.hpp"
#include "ProtobufMessageLoader.hpp"

using SystemProvider = ConstantDataProvider<
	data::SystemData, System,
	ProtobufMessageCompileLoader<data::SystemData, data::SystemDataWrapper>,
	System::SystemVerifier>;

#endif
