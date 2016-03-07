

#ifndef __PLAYERACCOUNT_HPP__
#define __PLAYERACCOUNT_HPP__

#include "websock/server_ws.hpp"
#include "typedefs.hpp"

class PlayerAccount {
public:
	PlayerAccount(WSConnection connection)
		: m_connection(connection) {}
	
private:
	/*! The current connection the player has to the server; may
	 *  be null */
	WSConnection m_connection;
}; // end class PlayerAccount

#endif

