#ifndef player_cpp
#define player_cpp

#include "player.h"

player::player(board * boardP, int idP, int mode) {
	my_board = boardP;
	id = idP;
	actions = 1;
	coins = 0;
	buys = 1;
	merchant_token = 0;
	collection * coll = my_board->coll;
	switch(mode) {
		case 0: case 1: decide = new human(this, mode); break;
		case 2 : decide = new AI(this); break;
	}
	hand = new card_pile(my_board);
	discard = new card_pile(my_board);
	played = new card_pile(my_board);
	deck = new card_pile(my_board,new card(coll->ids->at("Copper")),7);
	deck->add(new card(coll->ids->at("Estate")),3);
	my_board->add_log("Player " + to_string(id) + (mode == 2 ? " AI" : " Human") + "\n");
	my_board->log_tab++;
	my_board->add_log("Starts with " + has()->count_string());
	deck->shuffle();
	my_board->add_log("Shuffles their deck\n");
	draw(5);
	my_board->log_tab--;
}

int player::vp() {
	card_pile * own = has();
	int ret = 0;
	for (card * c : *(own->pile)) {
		ret += my_board->coll->cards->at(c->id)->vp(this);
	}
	return ret;
}

card_pile * player::has() {
	card_pile * ret = new card_pile(my_board);
	ret->pile->reserve(hand->size() + deck->size() + discard->size() + played->size());
	ret->pile->insert(ret->pile->end(), hand->pile->begin(), hand->pile->end());
	ret->pile->insert(ret->pile->end(), deck->pile->begin(), deck->pile->end());
	ret->pile->insert(ret->pile->end(), discard->pile->begin(), discard->pile->end());
	ret->pile->insert(ret->pile->end(), played->pile->begin(), played->pile->end());
	return ret;
}

void player::draw(int ammount, bool self) {
	int drawn = 0;
	int last_drawn = 0;
	string intro = self ? "" : "Player " + to_string(id) + " ";
	for (int i = 0; i < ammount; i++) {
		if (deck->empty()) {
			if (!discard->empty()) {
				if (last_drawn > 0) {
					my_board->add_log(intro + "Draws " + to_string(last_drawn) + (last_drawn == 1 ? " Card\n" : " Cards\n"));
					last_drawn = 0;
				}
				vector<card*> * swap = deck->pile;
				deck->pile = discard->pile;
				discard->pile = swap;
				deck->shuffle();
				my_board->add_log(intro + "Shuffles their discard pile into their deck\n");
				hand->add(deck->pop());
				drawn++;
				last_drawn++;
			} else {
				my_board->add_log(intro + "Fails to draw " + to_string(ammount - drawn) + (ammount - drawn == 1 ? " Card\n" : " Cards\n"));
				break;
			}
		} else {
			hand->add(deck->pop());
			drawn++;
			last_drawn++;
		}
	}
	if (last_drawn > 0) {
		my_board->add_log(intro + "Draws " + to_string(last_drawn) + (last_drawn == 1 ? " Card\n" : " Cards\n"));
	}
}

int player::play_from_hand(int position) {
	if (position < 0 || position >= hand->size()) return -1;
	card_data * card = my_board->coll->cards->at(hand->at(position)->id);
	if (my_board->phase == Action && card->types.count("Action") == 1) {
		if (actions == 0) {
			return -3;
		}
		actions--;
		play(hand->remove(position));
		return card->id;
	} else if (my_board->phase == Treasure && card->types.count("Treasure") == 1) {
		play(hand->remove(position));
		return card->id;
	} else return -2;
}

int player::play_by_id(int id) {
	int i = 0;
	while (i < hand->size() && hand->at(i)->id != id) {
		i++;
	}
	if (i == hand->size()) {
		return -4;
	} else {
		return play_from_hand(i);
	}
}

void player::play(card * cardP, bool add_to_play) {
	card_data * data = my_board->coll->cards->at(cardP->id);
	my_board->add_log("Plays " + data->name + "\n");
	my_board->log_tab++;
	if (add_to_play) {
		played->add(cardP);
	}
	data->play(this);
	my_board->log_tab--;
}

int player::buy(int card) {
	if (buys == 0) return -1;
	if (my_board->phase != Buy) return -2;
	if (my_board->supply->pile_size(card) == 0) return -3;
	card_data * data = my_board->coll->cards->at(card);
	if (coins < data->cost) return -4;
	buys--;
	coins -= data->cost;
	my_board->add_log("Buys a " + data->name + "\n");
	my_board->log_tab++;
	discard_card(my_board->supply->pop(card));
	my_board->log_tab--;
	return card;
	
}

bool player::gain(int card, bool self) {
	bool ret = (discard_card(my_board->supply->pop(card)) != nullptr);
	string name = my_board->coll->cards->at(card)->name;
	if (ret) {
		if (self) {
			my_board->add_log("Gains a " + name + "\n");
		} else {
			my_board->add_log("Player " + to_string(id) + " gains a " + name + "\n");
		}
	}
	return ret;
}

bool player::gain_to_hand(int card, bool self) {
	bool ret = hand->add(my_board->supply->pop(card)) != nullptr;
	string name = my_board->coll->cards->at(card)->name;
	if (ret) {
		if (self) {
			my_board->add_log("Gains a " + name + " to hand\n");
		} else {
			my_board->add_log("Player " + to_string(id) + " gains a " + name + " to hand\n");
		}
	}
	return ret;
}

bool player::gain_to_deck(int card, bool self) {
	bool ret = deck->add(my_board->supply->pop(card)) != nullptr;
	string name = my_board->coll->cards->at(card)->name;
	if (ret) {
		if (self) {
			my_board->add_log("Gains a " + name + " to hand\n");
		} else {
			my_board->add_log("Player " + to_string(id) + " gains a " + name + " to hand\n");
		}
	}
	return ret;
}

card * player::discard_card(card * to_disc) {
	discard->add(to_disc);
	return to_disc;
}

card * player::top_deck() {
	if (deck->empty()) {
		if (!discard->empty()) {
			vector<card*> * swap = deck->pile;
			deck->pile = discard->pile;
			discard->pile = swap;
			deck->shuffle();
			my_board->add_log("Shuffles their discard pile into their deck\n");
			return deck->pop();
		} else {
			return nullptr;
		}
	} else {
		return deck->pop();
	}
}

void player::cleanup() {
	for (int i = played->size() - 1; i >= 0; i--) {
		discard_card(played->remove(i));
	}
	for (int i = hand->size() - 1; i >= 0; i--) {
		discard_card(hand->remove(i));
	}
	actions = 1;
	coins = 0;
	buys = 1;
	merchant_token = 0;
	my_board->add_log("Discards hand and played cards, resets to 1 Action, 0 Coins and 1 Buy\n");
	draw(5);
}

bool player::attack(int code) {
	collection * coll = my_board->coll;
	int ret = true;
	for (int i = hand->size() - 1; i >= 0; i--) {
		card_data * data = coll->cards->at(hand->at(i)->id);
		bool res = (data->react_to == "attack" ? data->react(this,i,code) : true);
		ret = ret && res;
	}
	return ret;
}

#endif