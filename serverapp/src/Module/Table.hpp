

#ifndef __TABLE_HPP__
#define __TABLE_HPP__

#include "GameServer.hpp"
#include "Table.pb.h"

class DataModule;

class Table {
public:
	using CardIdMap_type = std::vector<int>;
public:
	Table();

	static inline void setDataProvider(DataModule& dataProvider) {
		Table::data = &dataProvider;
	}

	void initialize(int id);

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
