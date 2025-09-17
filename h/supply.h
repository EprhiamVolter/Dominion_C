#ifndef supply_h
#define supply_h

#include <iostream>
#include <map>
#include <vector>

using namespace std;

class board;
class card_pile;
class card;

class supplyT {
	public:
		board * my_board;
		map<int, card_pile*> * piles;
		int empty_piles;
		
		supplyT(board * boardP, bool set_kingdom = false, vector<int> kingdom = vector<int>());
		int pile_size(int card);
		card * pop(int card);
		card_pile * has_buy_for(int coins);
		card_pile * gain(vector<int> ids);
		string to_string();
};

#include "board.h"
#include "card_pile.h"
#include "card.h"

#endif