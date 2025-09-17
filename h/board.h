#ifndef board_h
#define board_h

#include <random>
#include <iostream>
#include <vector>

using namespace std;

class collection;
class player;
class supplyT;
class card_pile;
class card;

enum phases {
	Action,
	Treasure,
	Buy,
	Cleanup,
	Ended
};

string phase_name(phases phase);

struct log_entry {
	string report;
	int round;
	int visibility;
};

class board {
	public:
		mt19937 gen;
		collection * coll;
		int round;
		int turn;
		phases phase;
		vector<player*> * players;
		int player_count;
		supplyT * supply;
		card_pile * trash;
		bool last_round;
		vector<log_entry> * log;
		int log_tab;
		bool print_log;
		
		board(collection * collP, int autopass = 2, int AIs = 0, int manual = 0, bool print_logP = false, bool set_kingdom = false, vector<int> kingdom = vector<int>());
		void add_log(string report);
		phases end_phase();
		bool has_play();
		void reveal(card * revealed, int player);
		card * trash_card(card * trashed, int player);
		string to_string_all();
		string to_string_public();
		string to_string_for_player(int player);
		string to_string_log(int from = 0);
		string end_card();
};

#include "collection.h"
#include "player.h"
#include "supply.h"
#include "card_pile.h"
#include "card.h"

#endif