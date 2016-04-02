

#ifndef __NETUTILITY_HPP__
#define __NETUTILITY_HPP__

#include "typedefs.hpp"

namespace Utility {

inline std::string connectionString(const WSConnection& con) {
	return Utility::concat(con->remote_endpoint_address, ":",
						   con->remote_endpoint_port
						   //, " (", (size_t)con.get(),
//						   ")"
		);
}
} // end namespace Utility

#endif
