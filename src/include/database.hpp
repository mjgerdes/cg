

#ifndef __DB_DATABASE_HPP__
#define __DB_DATABASE_HPP__

#include <string>
#include <memory>   // std::auto_ptr
#include <cstdlib>  // std::exit
#include <iostream>
#include <odb/connection.hxx>
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>

#include <odb/database.hxx>

#if defined(DATABASE_MYSQL)
#include <odb/mysql/database.hxx>
#elif defined(DATABASE_SQLITE)
#include <odb/sqlite/database.hxx>
#elif defined(DATABASE_PGSQL)
#include <odb/pgsql/database.hxx>
#elif defined(DATABASE_ORACLE)
#include <odb/oracle/database.hxx>
#elif defined(DATABASE_MSSQL)
#include <odb/mssql/database.hxx>
#else
#error unknown database; did you forget to define the DATABASE_* macros?
#endif

#include <odb/connection.hxx>
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>

namespace db {

using DBServer = odb::database;
using DBServer_ptr = std::unique_ptr<DBServer>;

inline DBServer_ptr makeDatabaseServer() {
	using namespace odb::core;

#if defined(DATABASE_MYSQL)
	auto db = DBServer_ptr{
		new odb::mysql::database("cgserver", "password", "cgserver_db")};
#endif

	{
		connection_ptr c(db->connection());

		transaction t(c->begin());
		schema_catalog::create_schema(*db);
		t.commit();
	}

	return db;
}  // end makeDatabaseConnection

}  // end namespace db

#endif
