

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

msg::ObfuscationTableMessage Table::player1ObfuscationTableMessage() const {
	return this->foldPlayer1Cards(
		msg::ObfuscationTableMessage{}, [this](auto& msg, auto obfuscatedId) {
			auto entry = msg.add_obfuscation_table_entry();
			entry->set_obfuscated(obfuscatedId);
			entry->set_real(this->m_cardMap.at(obfuscatedId));
			return std::move(msg);
		});
}


msg::ObfuscationTableMessage Table::player2ObfuscationTableMessage() const {
	return this->foldPlayer2Cards(
		msg::ObfuscationTableMessage{}, [this](auto& msg, auto obfuscatedId) {
			auto entry = msg.add_obfuscation_table_entry();
			entry->set_obfuscated(obfuscatedId);
			entry->set_real(this->m_cardMap.at(obfuscatedId));
			return std::move(msg);
		});
}


Table::~Table(){};
