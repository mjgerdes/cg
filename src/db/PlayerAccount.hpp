
#ifndef __DB_PLAYERACCOUNT_HPP__
#define __DB_PLAYERACCOUNT_HPP__ #ifndef __DB_PLAYERACCOUNT_HPP__

#include <odb/core.hxx>
#include "typedefs.hpp"


namespace db {




#pragma db object
class PlayerAccount {
public:
	// constructor
	PlayerAccount(WSConnection& currentConnection) : m_currentConnection(currentConnection) {}
	
private:
	friend class odb::access;

	PlayerAccount() {}

#pragma db id auto
	unsigned long m_id;
	std::string m_email;
	std::string m_name;

#pragma db member transient
	WSConnection m_currentConnection;
};  // end class PlayerAccount

} // end namespace db


#endif
