#ifndef supply_cpp
#define supply_cpp

#include "supply.h"

supplyT::supplyT(board * boardP, bool set_kingdom, vector<int> kingdom) {
	my_board = boardP;
	my_board->add_log("Supply\n");
	my_board->log_tab++;
	collection * coll = my_board->coll;
	int player_count = my_board->player_count;
	empty_piles = player_count > 1 ? 0 : 1;
	piles = new map<int, card_pile*>;
	piles->emplace(coll->ids->at("Copper"),new card_pile(my_board,new card(coll->ids->at("Copper")),60 - player_count * 7));
	piles->emplace(coll->ids->at("Silver"),new card_pile(my_board,new card(coll->ids->at("Silver")),40));
	piles->emplace(coll->ids->at("Gold"),new card_pile(my_board,new card(coll->ids->at("Gold")),30));
	int victory_ammount = player_count > 2 ? 12 : 8;
	piles->emplace(coll->ids->at("Estate"),new card_pile(my_board,new card(coll->ids->at("Estate")),victory_ammount));
	piles->emplace(coll->ids->at("Duchy"),new card_pile(my_board,new card(coll->ids->at("Duchy")),victory_ammount));
	piles->emplace(coll->ids->at("Province"),new card_pile(my_board,new card(coll->ids->at("Province")),victory_ammount));
	piles->emplace(coll->ids->at("Curse"),new card_pile(my_board,new card(coll->ids->at("Curse")),10 * (player_count - 1)));
	string Victory_string = player_count > 2 ? "12" : "8";
	my_board->add_log("Basic - " + std::to_string(60 - player_count * 7) + " Copper, 40 Silver, 30 Gold, "
	+ Victory_string + " Estates, " +  Victory_string + " Duchies, " + Victory_string + " Provinces, "
	+ std::to_string(10 * (player_count - 1)) + " Curses\n");
	vector<int> kingdom_piles;
	if (set_kingdom) {
		kingdom_piles = kingdom;
	} else {
		kingdom_piles = coll->kingdom();
	}
	string report = "Kingdom - ";
	for (int i = 0; i < kingdom_piles.size(); i++) {
		int id = kingdom_piles.at(i);
		card_data * data = coll->cards->at(id);
		piles->emplace(id,new card_pile(my_board,new card(id),(data->types.count("Victory") == 1 ? victory_ammount : 10)));
		report += std::to_string(data->types.count("Victory") == 1 ? victory_ammount : 10) + " " + data->name + ", ";
	}
	report.pop_back();
	report.pop_back();
	my_board->add_log(report + "\n");
	my_board->log_tab--;
}

int supplyT::pile_size(int card) {
	if (piles->count(card) == 0) {
		return 0;
	}
	return piles->at(card)->size();
}

card * supplyT::pop(int card) {
	if (piles->count(card) == 0) {
		return nullptr;
	} //TODO check if not needed
	if (pile_size(card) == 1) {
		empty_piles++;
		if ((empty_piles == 3 && my_board->player_count < 5) || empty_piles == 4 ||card == my_board->coll->ids->at("Province")) {
			my_board->last_round = true;
		}
	}
	return piles->at(card)->pop();
}

card_pile * supplyT::gain(vector<int> ids) {
	card_pile * ret = new card_pile(my_board);
	for (int id : ids) {
		ret->add(pop(id));
	}
	return ret;
}

card_pile * supplyT::has_buy_for(int coins) {
	card_pile * ret = new card_pile(my_board);
	for (pair<int,card_pile*> pile : *piles) {
		if (pile.second->size() > 0 && my_board->coll->cards->at(pile.first)->cost <= coins) {
			ret->add(new card(pile.first));
		}
	}
	return ret;
}

string supplyT::to_string() {
	string ret;
	if (!piles->empty()) {
		for (pair<int,card_pile*> pile : *piles) {
			ret += std::to_string(pile.second->size()) + " " + my_board->coll->cards->at(pile.first)->name + ", ";
		}
		ret.pop_back();
		ret.pop_back();
	}
	return ret + "\n";
	
}

#endif