
#ifndef __DB_PLAYERACCOUNT_HPP__
#define __DB_PLAYERACCOUNT_HPP__ #ifndef __DB_PLAYERACCOUNT_HPP__

#include <odb/core.hxx>
#include "typedefs.hpp"
//#pragma db namespace pointer(std::unique_ptr)
namespace db {

#pragma db object
class PlayerAccount {
public:
	using Id_type = unsigned long;
//	using Id_type = int;
	PlayerAccount(const std::string& email, const std::string& password) : m_email(email), m_password(password), m_name("") {}

	inline Id_type id() const {
		return m_id;
	}

	inline std::string email() const {
		return m_email;
	}
	
private:
	friend class odb::access;

	PlayerAccount() {}

#pragma db id auto
	Id_type m_id;
	std::string m_email;
	std::string m_password;
	std::string m_name;
};  // end class PlayerAccount

}  // end namespace db

#endif
