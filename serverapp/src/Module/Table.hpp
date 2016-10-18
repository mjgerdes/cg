

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
	msg::ObfuscationTableMessage player1ObfuscationTableMessage() const;
		msg::ObfuscationTableMessage player2ObfuscationTableMessage() const; 
	msg::ObfuscationTableMessage player1HandObfuscationTableMessage() const;
	msg::ObfuscationTableMessage player2HandObfuscationTableMessage() const;
	~Table();

	inline const msg::Table& raw() const { return m_model; }

public:
	// templates

	template <typename T, typename F>
	T foldPlayer1HandCards(T identity, F f) const {
		auto acc = identity;
				for (int i = 0; i < m_model.player1_hand_size(); ++i) {
			acc = f(acc, m_model.player1_hand(i));
		}
				return acc;
	} // end foldPlayer1HandCards

	template <typename T, typename F>
	T foldPlayer2HandCards(T identity, F f) const {
		auto acc = identity;
				for (int i = 0; i < m_model.player2_hand_size(); ++i) {
			acc = f(acc, m_model.player2_hand(i));
		}
				return acc;
	} // end foldPlayer2HandCards
	
	template <typename T, typename F>
	T foldPlayer1Cards(T identity, F f) const {
		auto acc = identity;
		for (int i = 0; i < m_model.player1_hand_size(); ++i) {
			acc = f(acc, m_model.player1_hand(i));
		}

		//cards in systems
		for(int i = 0; i < m_model.player1_system_cards_size(); ++i) {
			for(int j = 0; j < m_model.player1_system_cards(i).obfuscated_card_id_size(); ++j) {
				acc = f(acc, m_model.player1_system_cards(i).obfuscated_card_id(j));
			}
		}

		// crew cards
		for(int i = 0; i < m_model.player1_crew_cards_size(); ++i) {
			acc = f(acc, m_model.player1_crew_cards(i));
		}

		// space cards
		for(int i = 0; i < m_model.player1_space_cards_size(); ++i) {
			acc = f(acc, m_model.player1_space_cards(i));
		}

		return acc;
	}  // end foldPlayer1Cards


	template <typename T, typename F>
	T foldPlayer2Cards(T identity, F f) const {
		auto acc = identity;
		for (int i = 0; i < m_model.player2_hand_size(); ++i) {
			acc = f(acc, m_model.player2_hand(i));
		}

		//cards in systems
		for(int i = 0; i < m_model.player2_system_cards_size(); ++i) {
			for(int j = 0; j < m_model.player2_system_cards(i).obfuscated_card_id_size(); ++j) {
				acc = f(acc, m_model.player2_system_cards(i).obfuscated_card_id(j));
			}
		}

		// crew cards
		for(int i = 0; i < m_model.player2_crew_cards_size(); ++i) {
			acc = f(acc, m_model.player2_crew_cards(i));
		}

		// space cards
		for(int i = 0; i < m_model.player2_space_cards_size(); ++i) {
			acc = f(acc, m_model.player2_space_cards(i));
		}

		return acc;
	}  // end foldPlayer2Cards

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
