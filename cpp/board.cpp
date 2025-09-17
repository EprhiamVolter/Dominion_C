#ifndef board_cpp
#define board_cpp

#include <algorithm>
#include <chrono>

#include "board.h"

string phase_name(phases p) {
	switch(p) {
		case Action		: return "Action";
		case Treasure	: return "Treasure";
		case Buy 		: return "Buy";
		case Cleanup	: return "Cleanup";
		case Ended		: return "Ended";
		default			: return "NULL";
	}
}

board::board(collection * collP, int autopass, int AIs, int manual, bool print_logP, bool set_kingdom, vector<int> kingdom) :
    gen(chrono::system_clock::now().time_since_epoch().count()) {
	coll = collP;
	round = 0;
	turn = 0;
	phase = Action;
	player_count = autopass + AIs + manual;
	trash = new card_pile(this);
	last_round = false;
	log = new vector<log_entry>;
	log_tab = 0;
	print_log = print_logP;
	add_log("Game Setup\n");
	log_tab++;
	supply = new supplyT(this,set_kingdom,kingdom);
	vector<int> mode(manual, 0);
	mode.insert(mode.end(), AIs, 2);
	mode.insert(mode.end(), autopass, 1);
	shuffle(mode.begin(),mode.end(),gen);
	players = new vector<player*>;
	for (int i = 0; i < player_count; i++) {
		player * p = new player(this,i,mode.at(i));
		players->push_back(p);
	}
	log_tab--;
	round++;
	if (player_count > 0) {
		add_log("\n");
		add_log("Round 1\n");
		log_tab++;
		add_log("Player 0" + string(players->at(0)->decide->human_player ? " Human" : " AI") + "\n");
		log_tab++;
		add_log("Action Phase\n");
		log_tab++;
	}
}

void board::add_log(string report) {
	log_entry next;
	next.visibility = -1;
	next.round = round;
	next.report = string(log_tab,'\t') + report;
	log->push_back(next);
	if (print_log) {
		cout << report;
	}
}

void board::reveal(card * revealed, int player) {
	if (player == -1) {
		add_log("Reveals a " + coll->cards->at(revealed->id)->name + "\n");
	} else {
		add_log("Player " + to_string(player) + " reveals a " + coll->cards->at(revealed->id)->name + "\n");
	}
}

card * board::trash_card(card * trashed, int player) {
	if (player == -2) {
		add_log(coll->cards->at(trashed->id)->name + " trashed from Supply\n");
	} else if (player == -1) {
		add_log("Trashes a " + coll->cards->at(trashed->id)->name + "\n");
	} else {
		add_log("Player " + to_string(player) + " trashes a " + coll->cards->at(trashed->id)->name + "\n");
	}
	trash->add(trashed);
	return trashed;
}

phases board::end_phase() {
	switch(phase) {
		case Action: {
			log_tab--;
			add_log("Treasure Phase\n");
			log_tab++;
			return phase = Treasure;
		}
		case Treasure: 	{
			log_tab--;
			add_log("Buy Phase\n");
			log_tab++;
			return phase = Buy;
		}
		case Buy: {
			log_tab--;
			add_log("Cleanup Phase\n");
			log_tab++;
			phase = Cleanup;
			players->at(turn)->cleanup();
			return end_phase();
		} 
		case Cleanup: {
			if (turn == player_count - 1) {
				log_tab -= 3;
				if (last_round) {
					add_log("Game End\n");
					return phase = Ended;
				} else if (round >= 1000) {
					add_log("Round Limit Game End\n");
					return phase = Ended;
				} else {
					round++;
					turn = 0;
					add_log("\n");
					add_log("Round " + to_string(round) + "\n");
					log_tab++;
				}
			} else {
				log_tab -= 2;
				turn++;
			}
			add_log("Player " + to_string(turn) + string(players->at(turn)->decide->human_player ? " Human" : " AI") + "\n");
			log_tab++;
			add_log("Action Phase\n");
			log_tab++;
			return phase = Action;
		}
		default: return Ended;
	}
}

bool board::has_play() {
	switch(phase) {
		case Action: {
			if (players->at(turn)->actions == 0) return false;
			card_pile * Actions = players->at(turn)->hand->filter_type("Action");
			bool ret = !Actions->empty();
			delete Actions;
			return ret;
		}
		case Treasure: 	{
			card_pile * Treasures = players->at(turn)->hand->filter_type("Treasure");
			bool ret = !Treasures->empty();
			delete Treasures;
			return ret;
		}
		case Buy: {
			if (players->at(turn)->buys == 0) return false;
			vector<card*> * affordables = supply->has_buy_for(players->at(turn)->coins)->pile;
			bool ret = !affordables->empty();
			delete affordables;
			return ret;
		}
		default: return false;
	}
}

string board::to_string_all() {
	string ret = "Players " + to_string(player_count) + ", Round " + to_string(round) + ", Turn Player " + to_string(turn) 
	+ ", " + phase_name(phase) + " Phase\nTrash\t" + trash->to_string() + "Supply\t" + supply->to_string();
	for (int i = 0; i < player_count; i++) {
		ret += "Player " + to_string(i) + (players->at(i)->decide->human_player ? " human" : " AI") + "\n\tVP " + to_string(players->at(i)->vp()) + ", Actions " +
		to_string(players->at(i)->actions) + ", Coins " + to_string(players->at(i)->coins) + ", Buys " + to_string(players->at(i)->buys) + "\n\tDeck\t" + 
		players->at(i)->deck->to_string() + "\tHand\t" +players->at(i)->hand->to_string() + "\tPlayed\t" + players->at(i)->played->to_string() + "\tDiscard " + 
		players->at(i)->discard->to_string();
	}
	return ret;
}

string board::to_string_public() {
	string ret = "Players " + to_string(player_count) + ", Round " + to_string(round) + ", Turn Player " + to_string(turn) 
	+ ", " + phase_name(phase) + " Phase\nTrash\t" + trash->to_string() + "Supply\t" + supply->to_string();
	for (int i = 0; i < player_count; i++) {
		ret += "Player " + to_string(i) + (players->at(i)->decide->human_player ? " human" : " AI") + "\n\tVP " + to_string(players->at(i)->vp()) + ", Actions " +
		to_string(players->at(i)->actions) + ", Coins " + to_string(players->at(i)->coins) + ", Buys " + to_string(players->at(i)->buys) + ", Deck " +
		to_string(players->at(i)->deck->size()) + ", Hand " + to_string(players->at(i)->hand->size()) + ", Played " + to_string(players->at(i)->played->size()) + ", Discard " + 
		to_string(players->at(i)->discard->size()) + "\n\tHas\t" + players->at(i)->has()->count_string();
	}
	return ret;
}

string board::to_string_for_player(int player) {
	string ret = "Players " + to_string(player_count) + ", Round " + to_string(round) + ", Turn Player " + to_string(turn) 
	+ ", " + phase_name(phase) + " Phase\nTrash\t" + trash->to_string() + "Supply\t" + supply->to_string();
	for (int i = 0; i < player; i++) {
		ret += "Player " + to_string(i) + (players->at(i)->decide->human_player ? " Human" : " AI") + "\n\tVP " + to_string(players->at(i)->vp()) + ", Actions " +
		to_string(players->at(i)->actions) + ", Coins " + to_string(players->at(i)->coins) + ", Buys " + to_string(players->at(i)->buys) + ", Deck " + 
		to_string(players->at(i)->deck->size()) + ", Hand " + to_string(players->at(i)->hand->size()) + ", Played " + to_string(players->at(i)->played->size()) + 
		", Discard " + to_string(players->at(i)->discard->size()) + "\n\tHas\t" + players->at(i)->has()->count_string();
	}
	for (int i = player + 1; i < player_count; i++) {
		ret += "Player " + to_string(i) + (players->at(i)->decide->human_player ? " Human" : " AI") + "\n\tVP " + to_string(players->at(i)->vp()) + ", Actions " + 
		to_string(players->at(i)->actions) + ", Coins " + to_string(players->at(i)->coins) + ", Buys " + to_string(players->at(i)->buys) + ", Deck " + 
		to_string(players->at(i)->deck->size()) + ", Hand " + to_string(players->at(i)->hand->size()) + ", Played " + to_string(players->at(i)->played->size()) + 
		", Discard " + to_string(players->at(i)->discard->size()) + "\n\tHas\t" + players->at(i)->has()->count_string();
	}
	return ret + "Player " + to_string(player) + (players->at(player)->decide->human_player ? " Human" : " AI") + " (You)\n\tVP " + to_string(players->at(player)->vp()) + 
	", Actions " + to_string(players->at(player)->actions) + ", Coins " + to_string(players->at(player)->coins) + ", Buys " + to_string(players->at(player)->buys) + "\n\tDeck\t" +
	players->at(player)->deck->count_string() + "\tHand\t" + players->at(player)->hand->to_string() + "\tPlayed\t" + players->at(player)->played->count_string() + "\tDiscard " + 
	players->at(player)->discard->count_string();
}

string board::to_string_log(int from) {
	string ret;
	int i = 0;
	while (i < log->size() && log->at(i).round < from) {
		i++;
	}
	while (i < log->size()) {
		ret += log->at(i).report;
		i++;
	}
	return ret;
}

bool wins(pair<int,int> x, pair<int,int> y) {
	if (x.first > y.first) {
		return true;
	} if (x.first < y.first) {
		return false;
	} else {
		return x.second > y.second;
	}
}

string board::end_card() {
	string ret;
	set<pair<int,int>,bool(*)(pair<int,int>,pair<int,int>)> podium (wins);
	for (player * p : *players) {
		podium.emplace(p->vp(),p->id);
	}
	ret = "\nPlaces  Winner  ";
	for (pair<int,int> p : podium) {
		ret += "Player " + to_string(p.second) + " with " + to_string(p.first) + " Victory Points\n\t\t"; 
	}
	ret.pop_back();
	ret.pop_back();
	return ret + "\n";
}

#endif