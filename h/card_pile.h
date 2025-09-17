#ifndef card_pile_h
#define card_pile_h

#include <iostream>
#include <vector>

using namespace std;

class board;
class card;

class card_pile {
	public:
		board * my_board;
		vector<card*> * pile;
		
		card_pile(board * boardP);
		card_pile(board * boardP, card * cardP, int ammount = 1);
		bool empty();
		int size();
		card * pop();
		card * remove(int position);
		card * add(card * cardP, int ammount = 1);
		void shuffle();
		card_pile * filter_type(string type);
		string to_string();
		string count_string();
		card * at(int position);
};

#include "board.h"
#include "card.h"

#endif