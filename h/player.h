#ifndef player_h
#define player_h

#include <iostream>

using namespace std;

class board;
class decider;
class card_pile;
class card;

class player {
	public:
		board * my_board;
		int id;
		decider * decide;
		card_pile * deck;
		card_pile * discard;
		card_pile * hand;
		card_pile * played;
		int actions;
		int coins;
		int buys;
		
		player(board * boardP, int idP, int mode = 1);
		int play_from_hand(int position);
		int play_by_id(int id);
		void play(card * cardP, bool add_to_played = true);
		int buy(int card);
		bool gain(int card, bool self = true);
		bool gain_to_hand(int card, bool self = true);
		bool gain_to_deck(int card, bool self = true);
		card * discard_card(card * to_disc);
		void cleanup();
		bool attack(int code);
		int vp();
		card_pile * has();
		void draw(int ammount = 1, bool self = true);
		card * top_deck();
		
		int merchant_token;
};

#include "board.h"
#include "decider.h"
#include "card_pile.h"
#include "card.h"

#endif