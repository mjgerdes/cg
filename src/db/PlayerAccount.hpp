
#ifndef __DB_PLAYERACCOUNT_HPP__
#define __DB_PLAYERACCOUNT_HPP__ #ifndef __DB_PLAYERACCOUNT_HPP__

#include <odb/core.hxx>
#include "typedefs.hpp"
//#pragma db namespace pointer(std::unique_ptr)
namespace db {

#pragma db object
class PlayerAccount {
public:
	using id_type = unsigned long;

	PlayerAccount(const std::string& email, const std::string& password) : m_email(email), m_password(password), m_name("") {}

	inline id_type id() {
		return m_id;
	}

private:
	friend class odb::access;

	PlayerAccount() {}

#pragma db id auto
	id_type m_id;
	std::string m_email;
	std::string m_password;
	std::string m_name;
};  // end class PlayerAccount

}  // end namespace db

#endif
