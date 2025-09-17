#ifndef collection_cpp
#define collection_cpp

#include <chrono>
#include <algorithm>

#include "collection.h"

// void
void nothing(player * playing) {
	
}

// 0 VP
int zero(player * playing) {
	return 0;
}

// +1 Coin
void Copper_play(player * playing) {
	playing->coins+= 1;
}

// +2 Coins
void Silver_play(player * playing) {
	playing->coins+= (2 + playing->merchant_token);
	playing->merchant_token = 0;
}

// +3 Coins 
void Gold_play(player * playing) {
	playing->coins+= 3;
}

// +1 VP 
int Estate_vp(player * playing) {
	return 1;
}

// +3 VP
int Duchy_vp(player * playing) {
	return 3;
}

// +6 VP
int Province_vp(player * playing) {
	return 6;
}

// -1 VP
int Curse_vp(player * playing) {
	return -1;
}

//Gain a card to your hand costing up to 5 Coins. Put a card from your hand onto your deck
void Artisan_play(player * playing) {
	board * b = playing->my_board;
	vector<card*> * gains = b->supply->has_buy_for(5)->pile;
	if (!gains->empty()) {
		vector<int> pos = playing->decide->choose_cards(gains,{},1,1,true,"Artisan: Gain a card to your hand costing up to 5 Coins. Put a card from your hand onto your deck.",7);
		playing->gain_to_hand(gains->at(pos.at(0))->id,true);
	}
	delete gains;
	if (!playing->hand->empty()) {
		vector<int> pos = playing->decide->choose_cards(playing->hand->pile,{},1,1,false,"Artisan: Put a card from your hand onto your deck.",7);
		card * choosen = playing->hand->remove(pos.at(0));
		playing->deck->add(choosen);
	}
}

//Gain a Gold. Each other player reveals the top 2 cards of their deck, trashes a revealed Treasure other than Copper, and discards the rest
void Bandit_play(player * playing) {
	board * b = playing->my_board;
	playing->gain(b->coll->ids->at("Gold"),true);
	for (int p = 0; p < b->player_count; p++) {
		if (p != playing->id && b->players->at(p)->attack(8)) {
			vector<card *> opt;
			for (int i = 0; i < 2; i++) {
				card * top = (b->players->at(p)->top_deck());
				if (top != nullptr) {
					card_data * data = b->coll->cards->at(top->id);
					b->reveal(top, p);
					if (data->types.count("Treasure") == 1 && top->id != b->coll->ids->at("Copper")) {
						opt.push_back(top);
					} else {
						b->players->at(p)->discard_card(top);
					}
				}
			}
			if (!opt.empty()) {
				vector<int> pos = b->players->at(p)->decide->choose_cards(&opt,{},1,1,false,
				"Bandit: Trash a revealed Treasure other than Copper, and discard the rest.",8);
				for (int i = 0; i < opt.size(); i++) {
					if (i == pos.at(0)) {
						b->trash_card(opt.at(i),p);
					} else {
						b->players->at(p)->discard_card(opt.at(i));
					}
				}
			}
		}
	}
}

//Gain a Silver onto your deck. Each other player reveals a Victory card from their hand and puts it onto their deck (or reveals a hand with no Victory cards)
void Bureaucrat_play(player * playing) {
	board * b = playing->my_board;
	collection * coll = b->coll;
	playing->gain_to_deck(b->coll->ids->at("Silver"),true);
	for (int p = 0; p < b->player_count; p++) {
		if (p != playing->id && b->players->at(p)->attack(9)) {
			vector<int> opt;
			card_pile * p_hand = b->players->at(p)->hand;
			for (int i = 0; i < p_hand->size(); i++) {
				if (coll->cards->at(p_hand->at(i)->id)->types.count("Victory") == 1) {
					opt.push_back(i);
				}
			}
			if (opt.empty()) {
				for (int i = 0; i < p_hand->size(); i++) {
					b->reveal(p_hand->at(i),p);
				}
			} else {
				vector<int> pos = b->players->at(p)->decide->choose_cards(p_hand->pile,opt,1,1,false,
				"Bureaucrat: reveal a Victory card from your hand and put it onto your deck.",9);
				b->reveal(p_hand->at(pos.at(0)),p);
				b->players->at(p)->deck->add(p_hand->remove(pos.at(0)));
			}
		}
	}
}


//+1 Action. Discard any number of cards. +1 Card per card discarded
void Cellar_play(player * playing) {
	playing->actions+= 1;
	card_pile * my_hand = playing->hand;
	if (!my_hand->empty()) {
		vector<int> pos = playing->decide->choose_cards(my_hand->pile,{},0,my_hand->size(),false,"Cellar: Discard any number of cards. +1 Card per card discarded.",10);
		sort(pos.begin(),pos.end());
		for (int i = pos.size()-1; i >= 0; i--) {
			playing->discard_card(my_hand->remove(pos.at(i)));
		}
		playing->my_board->add_log("Discards " + to_string(pos.size()) + " cards\n");
		playing->draw(pos.size());
	}
}

//Trash up to 4 cards from your hand
void Chapel_play(player * playing) {
	card_pile * my_hand = playing->hand;
	if (!my_hand->empty()) {
		int max = my_hand->size() > 4 ? 4 : my_hand->size();
		vector<int> pos = playing->decide->choose_cards(my_hand->pile,{},0,max,false,"Chapel: Trash up to 4 cards from your hand.",11);
		sort(pos.begin(),pos.end());
		for (int i = pos.size()-1; i >= 0; i--) {
			playing->my_board->trash_card(playing->hand->remove(pos.at(i)),-1);
		}
	}
}

//+4 Cards. +1 Buy. Each other player draws a card
void Council_Room_play(player * playing) {
	playing->draw(4);
	playing->buys+= 1;
	board * b = playing->my_board;
	for (int p = 0; p < b->player_count; p++) {
		if (p != playing->id) {
			b->players->at(p)->draw(1,false);
		}
	}
}

// +2 Actions. +1 Buy. +2 Coins
void Festival_play(player * playing) {
	playing->actions+= 2;
	playing->buys+= 1;
	playing->coins+= 2;
}

//Worth 1 VP per 10 cards you have (round down)
int Gardens_vp(player * playing) {
	return playing->has()->size()/10;
}
//+1 Card. +1 Action. Look through your discard pile. You may put a card from it onto your deck
void Harbinger_play(player * playing) {
	playing->draw(1);
	playing->actions+= 1;
	card_pile * my_disc = playing->discard;
	if (!my_disc->empty()) {
		vector<int> pos = playing->decide->choose_cards(my_disc->pile,{},0,1,false,
		"Harbinger: Look through your discard pile. You may put a card from it onto your deck.",15);
		if (!pos.empty()) {
			playing->deck->add(my_disc->remove(pos.at(0)));
		}
	}
}

// +2 Cards. +1 Action
void Laboratory_play(player * playing) {
	playing->draw(2);
	playing->actions+= 1;
}

//Draw until you have 7 cards in hand, skipping any Action cards you choose to; set those aside, discarding them afterwards
void Library_play(player * playing) {
	card_pile * my_hand = playing->hand;
	board * b = playing->my_board;
	vector<card*> aside;
	card * top;
	while (my_hand->size() < 7 && (top = playing->top_deck()) != nullptr) {
		card_data * data = b->coll->cards->at(top->id);
		if (data->types.count("Action") == 1) {
			if (playing->decide->may("Library: skip " + data->name + " or decline.\n",17)) {
				aside.push_back(top);
			} else {
				my_hand->add(top);
			}
		} else {
			my_hand->add(top);
		}
	}
	for (int i = 0; i < aside.size(); i++) {
		playing->discard_card(aside.at(i));
	}
}

// +1 Card. +1 Action. +1 Buy. +1 Coin
void Market_play(player * playing) {
	playing->draw(1);
	playing->actions += 1;
	playing->buys += 1;
	playing->coins += 1;
}

//+1 Card. +1 Action. The first time you play a Silver this turn, +1 Coin
void Merchant_play(player * playing) {
	playing->draw(1);
	playing->actions += 1;
	playing->merchant_token += 1;
}

//+2 Coins. Each other player discards down to 3 cards in hand
void Militia_play(player * playing) { 
	playing->coins += 2;
	board * b = playing->my_board;
	for (int p = 0; p < b->player_count; p++) {
		if (p != playing->id && b->players->at(p)->attack(20)) {
			card_pile * p_hand = b->players->at(p)->hand;
			if (p_hand->size() > 3) {
				int max = p_hand->size() - 3;
				vector<int> pos = b->players->at(p)->decide->choose_cards(p_hand->pile,{},max,max,false,"Militia: discards down to 3 cards in hand.",20);
				sort(pos.begin(),pos.end());
				for (int i = pos.size()-1; i >= 0; i--) {
					b->players->at(p)->discard_card(p_hand->remove(pos.at(i)));
				}
			}
		}
	}
}

//You may trash a Treasure from your hand. Gain a Treasure to your hand costing up to 3 Coins more than it
void Mine_play(player * playing) {
	card_pile * my_hand = playing->hand;
	vector<int> opt;
	collection * coll = playing->my_board->coll;
	for (int i = 0; i < my_hand->size(); i++) {
		if (coll->cards->at(my_hand->at(i)->id)->types.count("Treasure") == 1) {
			opt.push_back(i);
		}
	}
	if (!opt.empty()) {
		vector<int> pos = playing->decide->choose_cards(playing->hand->pile,opt,0,1,false,
		"Mine: You may trash a Treasure from your hand. Gain a Treasure to your hand costing up to 3 Coins more than it.",21);
		if (!pos.empty()) {
			card * choosen = my_hand->remove(pos.at(0));
			playing->my_board->trash_card(choosen,-1);
			card_data * data = coll->cards->at(choosen->id);
			vector<card*> * gains = playing->my_board->supply->has_buy_for(3 + data->cost)->filter_type("Treasure")->pile;
			if (!gains->empty()) {
				vector<int> pos2 = playing->decide->choose_cards(gains,{},1,1,true,
				"Mine: Gain a Treasure to your hand costing up to 3 Coins more than " +  data->name + " (" + to_string(3 + data->cost) + ").",21);
				playing->gain_to_hand(gains->at(pos2.at(0))->id,true);
			}
		}
	}
}

//+2 Cards. When another player plays an Attack card, you may first reveal this from your hand, to be unaffected by it
void Moat_play(player * playing) {
	playing->draw(2);
}

bool Moat_react(player * playing, int pos, int code) {
	board * b = playing->my_board;
	if (playing->decide->may("Moat: reveal this to be unaffected by " + b->coll->cards->at(code)->name + " or decline.\n",22)) {
		b->reveal(playing->hand->at(pos), playing->id);
		return false;
	} else {
		return true;
	}
}

//You may trash a Copper from your hand for +3 Coins
void Moneylender_play(player * playing) {
	card_pile * my_hand = playing->hand;
	vector<int> opt;
	int id_Copper = playing->my_board->coll->ids->at("Copper");
	for (int i = 0; i < my_hand->size(); i++) {
		if (my_hand->at(i)->id == id_Copper) {
			opt.push_back(i);
		}
	}
	if (!opt.empty()) {
		vector<int> pos = playing->decide->choose_cards(playing->hand->pile,opt,0,1,false,"Moneylender: You may trash a Copper from your hand for +3 Coins.",23);
		if (!pos.empty()) {
			playing->my_board->trash_card(playing->hand->remove(pos.at(0)), -1);
			playing->coins += 3;
		}
	}
}

//+1 Card. +1 Action. +1 Coin. Discard a card per empty Supply pile
void Poacher_play(player * playing) {
	playing->draw(1);
	playing->actions+= 1;
	playing->coins+= 1;
	card_pile * my_hand = playing->hand;
	int empty_supplies = playing->my_board->supply->empty_piles;
	if (!my_hand->empty() && empty_supplies > 0) {
		int max = my_hand->size() > empty_supplies ? empty_supplies : my_hand->size();
		vector<int> pos = playing->decide->choose_cards(my_hand->pile,{},max,max,false,
		"Poacher: Discard a card per empty Supply pile (" + to_string(empty_supplies) +").",24);
		sort(pos.begin(),pos.end());
		for (int i = pos.size()-1; i >= 0; i--) {
			playing->discard_card(playing->hand->remove(pos.at(i)));
		}
	}
}

//Trash a card from your hand. Gain a card costing up to 2 Coins more than it
void Remodel_play(player * playing) {
	card_pile * my_hand = playing->hand;
	if (!my_hand->empty()) {
		board * b = playing->my_board;
		vector<int> pos = playing->decide->choose_cards(my_hand->pile,{},1,1,false,
		"Remodel: Trash a card from your hand. Gain a card costing up to 2 Coins more than it.",25);
		card * choosen = my_hand->remove(pos.at(0));
		b->trash_card(choosen, -1);
		card_data * data = b->coll->cards->at(choosen->id);
		vector<card*> * gains = b->supply->has_buy_for(2 + data->cost)->pile;
		if (!gains->empty()) {
			vector<int> pos2 = playing->decide->choose_cards(gains,{},1,1,true,
			"Remodel: Gain a card costing up to 2 Coins more than " +  data->name + " (" + to_string(2 + data->cost) + ").",25);
			playing->gain(gains->at(pos2.at(0))->id,true);
		}
	}
}

//+1 Card. +1 Action. Look at the top 2 cards of your deck. Trash and/or discard any number of them. Put the rest back on top in any order
void Sentry_play(player * playing) {
	card_pile opt = card_pile(playing->my_board);
	playing->draw(1);
	playing->actions += 1;
	for (int i = 0; i < 2; i++) {
		card * top = (playing->top_deck());
		if (top != nullptr) {
				opt.add(top);
		}
	}
	if (!opt.empty()) {
		vector<int> pos = playing->decide->choose_cards(opt.pile,{},0,opt.size(),false,
		"Sentry: Trash any number of these. Discard any number of the rest. Put the rest back on top in any order.",26);
		sort(pos.begin(), pos.end());
		for (int i = pos.size()-1; i >= 0; i--) {
			playing->my_board->trash_card(opt.remove(pos.at(i)),-1);
		}
		if (!opt.empty()) {
			pos = playing->decide->choose_cards(opt.pile,{},0,opt.size(),false,
			"Sentry: Discard any number these. Put the rest back on top in any order.",26);
			sort(pos.begin(), pos.end());
			for (int i = pos.size()-1; i >= 0; i--) {
				playing->discard_card(opt.remove(pos.at(i)));
			}
			if (!opt.empty()) {
				pos = playing->decide->choose_cards(opt.pile,{},opt.size(),opt.size(),false,
				"Sentry: Put these back on top in any order.",26);
				for (int i = pos.size()-1; i >= 0; i--) {
					playing->deck->add(opt.remove(pos.at(i)));
				}
			}
		}
	}
}

// +3 Cards
void Smithy_play(player * playing) {
	playing->draw(3);
}

//You may play an Action card from your hand twice
void Throne_Room_play(player * playing) {
	vector<int> opt;
	card_pile * my_hand = playing->hand;
	collection * coll = playing->my_board->coll;
	for (int i = 0; i < my_hand->size(); i++) {
		if (coll->cards->at(my_hand->at(i)->id)->types.count("Action") == 1) {
			opt.push_back(i);
		}
	}
	if (!opt.empty()) {
		board * b = playing->my_board;
		vector<int> pos = playing->decide->choose_cards(playing->hand->pile,opt,0,1,false,"Throne_Room: You may play an Action card from your hand twice.",28);
		if (!pos.empty()) {
			card * choosen = playing->hand->remove(pos.at(0));
			playing->play(choosen,true);
			playing->play(choosen,false);
		}
	}
}

// +2 Coins. Discard the top card of your deck. If it's an Action card, you may play it.
void Vassal_play(player * playing) {
	playing->coins+= 2;
	card * dis = playing->top_deck();
	card_data * data = playing->my_board->coll->cards->at(dis->id);
	if (data->types.count("Action") == 1 && (playing->decide->may("Vassal: Play " + data->name + " or decline\n",29))) {
		playing->play(dis,true);
	} else {
		playing->discard_card(dis);
	}
}

// +1 Card. +2 Actions
void Village_play(player * playing) {
	playing->draw(1);
	playing->actions+= 2;
}

// +2 Cards. Each other player gains a Curse
void Witch_play(player * playing) {
	playing->draw(2);
	board * b = playing->my_board;
	int id_Curse = b->coll->ids->at("Curse");
	for (int p = 0; p < b->player_count; p++) {
		if (p != playing->id  && b->players->at(p)->attack(31)) {
			b->players->at(p)->gain(id_Curse,false);
		}
	}
}

// Gain a card costing up to 4 Coins
void Workshop_play(player * playing) {
	vector<card*> * opt = playing->my_board->supply->has_buy_for(4)->pile;
	if (!opt->empty()) {
		vector<int> pos = playing->decide->choose_cards(opt,{},1,1,true,"Workshop: Gain a card costing up to 4 Coins.",32);
		playing->gain(opt->at(pos.at(0))->id,true);
	}
}

collection::collection(bool load_allP) {
	cards = new map<int, card_data*>;
	ids = new map<string, int>;
	loaded = new set<string>;
	
	if (load_allP) {
		load_all();
	}
}

vector<int> collection::kingdom() { // TODO make it choose according to loaded sets 
	static mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
	vector<int> implemented;
	for(int i = 7; i <= 32; i++) {
		implemented.push_back(i);
	}
	shuffle(implemented.begin(), implemented.end(), gen);
	vector<int> ret = vector<int>(10);
	copy(implemented.begin(), implemented.begin() + 10, ret.begin());
	
	return ret;
}

void collection::load_card(card_data * card) {
	cards->emplace(card->id,card);
	ids->emplace(card->name,card->id);
}

void collection::load_sets(set<string> sets) {
	if (sets.count("Standard") == 1 && loaded->count("Standard") == 0) {
		loaded->insert("Standard");
		load_card(new card_data(0,"Copper","Standard",0,{"Treasure"},Copper_play,zero,"1 Coin."));
		load_card(new card_data(1,"Silver","Standard",3,{"Treasure"},Silver_play,zero,"2 Coins."));
		load_card(new card_data(2,"Gold","Standard",6,{"Treasure"},Gold_play,zero,"3 Coins."));
		load_card(new card_data(3,"Estate","Standard",2,{"Victory"},nothing,Estate_vp,"1 VP."));
		load_card(new card_data(4,"Duchy","Standard",5,{"Victory"},nothing,Duchy_vp,"3 VP."));
		load_card(new card_data(5,"Province","Standard",8,{"Victory"},nothing,Province_vp,"6 VP."));
		load_card(new card_data(6,"Curse","Standard",0,{"Curse"},nothing,Curse_vp,"-1 VP."));
	}
	if (sets.count("Base") == 1 && loaded->count("Base") == 0) {
		loaded->insert("Base");
		load_card(new card_data(7,"Artisan","Base",6,{"Action"},Artisan_play,zero,"Gain a card to your hand costing up to 5 Coins. Put a card from your hand onto your deck."));
		load_card(new card_data(8,"Bandit","Base",5,{"Action","Attack"},Bandit_play,zero,"Gain a Gold. Each other player reveals the top 2 cards of their deck, trashes a revealed Treasure other than Copper, and discards the rest."));
		load_card(new card_data(9,"Bureaucrat","Base",4,{"Action","Attack"},Bureaucrat_play,zero,"Gain a Silver onto your deck. Each other player reveals a Victory card from their hand and puts it onto their deck (or reveals a hand with no Victory cards)."));
		load_card(new card_data(10,"Cellar","Base",2,{"Action"},Cellar_play,zero,"+1 Action. Discard any number of cards. +1 Card per card discarded."));
		load_card(new card_data(11,"Chapel","Base",2,{"Action"},Chapel_play,zero,"Trash up to 4 cards from your hand."));
		load_card(new card_data(12,"Council_Room","Base",5,{"Action"},Council_Room_play,zero,"+4 Cards. +1 Buy. Each other player draws a card."));
		load_card(new card_data(13,"Festival","Base",5,{"Action"},Festival_play,zero,"+2 Actions. +1 Buy. +2 Coins"));
		load_card(new card_data(14,"Gardens","Base",4,{"Victory"},nothing,Gardens_vp,"Worth 1 VP per 10 cards you have (round down)."));
		load_card(new card_data(15,"Harbinger","Base",3,{"Action"},Harbinger_play,zero,"+1 Card. +1 Action. Look through your discard pile. You may put a card from it onto your deck."));
		load_card(new card_data(16,"Laboratory","Base",5,{"Action"},Laboratory_play,zero,"+2 Cards. +1 Action"));
		load_card(new card_data(17,"Library","Base",5,{"Action"},Library_play,zero,"Draw until you have 7 cards in hand, skipping any Action cards you choose to; set those aside, discarding them afterwards."));
		load_card(new card_data(18,"Market","Base",5,{"Action"},Market_play,zero,"+1 Card. +1 Action. +1 Buy. +1 Coin"));
		load_card(new card_data(19,"Merchant","Base",3,{"Action"},Merchant_play,zero,"+1 Card. +1 Action. The first time you play a Silver this turn, +1 Coin."));
		load_card(new card_data(20,"Militia","Base",4,{"Action","Attack"},Militia_play,zero,"+2 Coins. Each other player discards down to 3 cards in hand."));
		load_card(new card_data(21,"Mine","Base",5,{"Action"},Mine_play,zero,"You may trash a Treasure from your hand. Gain a Treasure to your hand costing up to 3 Coins more than it."));
		load_card(new card_data(22,"Moat","Base",2,{"Action","Reaction"},Moat_play,zero,"+2 Cards. When another player plays an Attack card, you may first reveal this from your hand, to be unaffected by it.","attack",Moat_react));
		load_card(new card_data(23,"Moneylender","Base",4,{"Action"},Moneylender_play,zero,"You may trash a Copper from your hand for +3 Coins."));
		load_card(new card_data(24,"Poacher","Base",4,{"Action"},Poacher_play,zero,"+1 Card. +1 Action. +1 Coin. Discard a card per empty Supply pile."));
		load_card(new card_data(25,"Remodel","Base",4,{"Action"},Remodel_play,zero,"Trash a card from your hand. Gain a card costing up to 2 Coins more than it."));
		load_card(new card_data(26,"Sentry","Base",5,{"Action"},Sentry_play,zero,"+1 Card. +1 Action. Look at the top 2 cards of your deck. Trash and/or discard any number of them. Put the rest back on top in any order."));
		load_card(new card_data(27,"Smithy","Base",4,{"Action"},Smithy_play,zero,"+3 Cards"));
		load_card(new card_data(28,"Throne_Room","Base",4,{"Action"},Throne_Room_play,zero,"You may play an Action card from your hand twice."));
		load_card(new card_data(29,"Vassal","Base",3,{"Action"},Vassal_play,zero,"+2 Coins. Discard the top card of your deck. If it's an Action card, you may play it."));
		load_card(new card_data(30,"Village","Base",3,{"Action"},Village_play,zero,"+1 Card. +2 Actions"));
		load_card(new card_data(31,"Witch","Base",5,{"Action","Attack"},Witch_play,zero,"+2 Cards. Each other player gains a Curse."));
		load_card(new card_data(32,"Workshop","Base",3,{"Action"},Workshop_play,zero,"Gain a card costing up to 4 Coins."));
	}
}

void collection::load_all() {
	load_sets({"Standard","Base"});
}

#endif