

#include "Module/Table.hpp"

DataModule* Table::data = nullptr;

Table::Table() {};


void Table::initialize(int id) {
	m_model.Clear();
	m_cardMap.clear();
// FIXME: create new mapping
	m_model.set_unsafe_id(id);
	
}

void Table::associate(short obfuscated, Card::Id_type real) {
	m_cardMap[obfuscated] = real;
}
Table::~Table() {};
