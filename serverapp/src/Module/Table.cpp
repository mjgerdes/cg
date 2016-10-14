

#include "Module/Table.hpp"

DataModule* Table::data = nullptr;

Table::Table() {};


void Table::initialize(int id) {
	m_model.Clear();
// FIXME: create new mapping
	m_model.set_unsafe_id(id);
	
}

Table::~Table() {};
