

#ifndef __SYSTEMPROVIDER_HPP__
#define __SYSTEMPROVIDER_HPP__

#include "ConstantDataProvider.hpp"
#include "System.hpp"
#include "ProtobufMessageLoader.hpp"

using SystemProvider = ConstantDataProvider<
	data::SystemData, System,
	ProtobufMessageCompileLoader<data::SystemData, data::SystemDataWrapper>,
	System::SystemVerifier>;

#endif
