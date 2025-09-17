#ifndef AI_cpp
#define AI_cpp

#include <algorithm>

#include "decider.h"

AI::AI(player * playingP) {
	playing = playingP;
	human_player = false;
}

// current AI : only end phase if it has no plays, chooses a play randomly.
// except for buy, never buys Curse or Copper, buys Gold->Silver and Province->Duchy->Estate when enough gold.
// does not prioritize higher cost Actions when buying, or +1 Action when playing
// TODO improve

int AI::play() { 
	board * b = playing->my_board;
	if (!b->has_play()) {
		b->end_phase();
		return -1;
	}
	switch(b->phase) {
		case Action: {
			vector<int> Actions_pos;
			for (int i = 0; i < playing->hand->size(); i++) {
				if (b->coll->cards->at(playing->hand->at(i)->id)->types.count("Action") == 1) {
					Actions_pos.push_back(i);
				}
			}
			uniform_int_distribution<int> dist(0, Actions_pos.size() - 1);
			return playing->play_from_hand(Actions_pos.at(dist(b->gen)));
		}
		case Treasure: {
			vector<int> Treasures_pos;
			for (int i = 0; i < playing->hand->size(); i++) {
				if (b->coll->cards->at(playing->hand->at(i)->id)->types.count("Treasure") == 1) {
					Treasures_pos.push_back(i);
				}
			}
			uniform_int_distribution<int> dist(0, Treasures_pos.size() - 1);
			return playing->play_from_hand(Treasures_pos.at(dist(b->gen)));
		}
		case Buy : {
			vector<card *> * affordables = b->supply->has_buy_for(playing->coins)->pile;
			for (int i = 0; i < affordables->size(); i++) {
				if (affordables->at(i)->id == 6 || affordables->at(i)->id == 0) {
					affordables->erase(affordables->begin() + i);
					i--;
				}
			}
			int to_buy;
			if (affordables->size() > 0) {
				uniform_int_distribution<int> dist(0, affordables->size() - 1);
				to_buy = affordables->at(dist(b->gen))->id;
				if (to_buy == 1 && playing->coins >= 6 && b->supply->pile_size(2) > 0) {
					to_buy = 2;
				}
				if (to_buy == 3 && playing->coins >= 5 && b->supply->pile_size(4) > 0) {
					to_buy = 4;
				}
				if (to_buy == 4 && playing->coins >= 8 && b->supply->pile_size(5) > 0) {
					to_buy = 5;
				}
				return playing->buy(to_buy);
			} else {
				b->end_phase();
				return -1;
			}
		}
		default : {
			cout << "ERROR at AI.cpp\n";
			throw;
			return -1;
		}
	}
}

vector<int> AI::choose_cards(vector<card*> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) {
	board * b = playing->my_board;
	if (pos.size() == 0) {
		for (int i = 0; i < from->size(); i++) {
			pos.push_back(i);
		}
	}
	uniform_int_distribution<int> dist(min, max);
	int ammount = dist(b->gen);
	vector<int> ret;
	ret.reserve(ammount);
	if (repetitive) {
		uniform_int_distribution<int> dist2(0, pos.size() - 1);
		for (int i = 0; i < ammount; i++) {
			ret.push_back(pos.at(dist2(b->gen)));
		}
	} else {
		shuffle(pos.begin(), pos.end(), b->gen);
		for (int i = 0; i < ammount; i++) {
			ret.push_back(pos.at(i));
		}
	}
	return ret;
}

bool AI::may(string message, int id) {
	return true;
}


#endif