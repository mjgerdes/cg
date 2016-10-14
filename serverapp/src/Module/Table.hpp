

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
	~Table();

private:
	friend class TableServer;
	static DataModule* data;

	msg::Table m_model;
	CardIdMap_type m_cardMap;
	WSConnection m_player1;
	WSConnection m_player2;

};  // end class Table

#endif
