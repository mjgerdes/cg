

#ifndef __TABLE_HPP__
#define __TABLE_HPP__

#include <map>
#include "GameServer.hpp"
#include "Table.pb.h"
#include "Card.hpp"

class DataModule;

class Table {
public:
	using CardIdMap_type = std::map<short, Card::Id_type>;

public:
	Table();

	static inline void setDataProvider(DataModule& dataProvider) {
		Table::data = &dataProvider;
	}

	void initialize(int id);
	void associate(short obfuscated, Card::Id_type real);
		msg::ObfuscationTableMessage obfuscationTableMessage() const;
	~Table();

	inline const msg::Table& raw() const { return m_model; }

private:


private:
	friend class TableServer;
	static DataModule* data;

	msg::Table m_model;
	CardIdMap_type m_cardMap;

	// FIXME: need to change these to weak_ptr or we will be leaking
	// memory/holding the connection handles forever or until they get replaced
	WSConnection m_player1;
	WSConnection m_player2;

};  // end class Table

#endif
