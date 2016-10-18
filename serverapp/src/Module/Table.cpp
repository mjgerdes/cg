

#include "Module/Table.hpp"

DataModule* Table::data = nullptr;

Table::Table(){};

void Table::initialize(int id) {
	m_model.Clear();
	m_cardMap.clear();
	// FIXME: create new mapping
	m_model.set_unsafe_id(id);
}

void Table::associate(short obfuscated, Card::Id_type real) {
	m_cardMap[obfuscated] = real;
}  // end associate

msg::ObfuscationTableMessage Table::obfuscationTableMessage() const {
	auto msg = msg::ObfuscationTableMessage{};
	for (const auto& pair : m_cardMap) {
		auto entry = msg.add_obfuscation_table_entry();
		entry->set_obfuscated(pair.first);
		entry->set_real(pair.second);
	}

	return std::move(msg);
}  // end obfuscationTableMessage

Table::~Table(){};
