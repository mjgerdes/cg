

#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include "SystemData.pb.h"

class System {
public:
	using Id_type = data::SystemData::SystemId;
	using data_type = data::SystemData;

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

private:
	const data_type* m_data;
};  // end class system
#endif
