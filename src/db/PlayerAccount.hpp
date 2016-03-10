
#ifndef __DB_PLAYERACCOUNT_HPP__
#define __DB_PLAYERACCOUNT_HPP__ #ifndef __DB_PLAYERACCOUNT_HPP__

#include <odb/core.hxx>

namespace db {
#pragma db object
class PlayerAccount {
public:
	
private:
	friend class odb::access;

	PlayerAccount() {}

#pragma db id auto
	unsigned long m_id;
};  // end class PlayerAccount

} // end namespace db


#endif
