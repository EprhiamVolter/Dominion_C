#ifndef card_pile_cpp
#define card_pile_cpp

#include <algorithm>

#include "card_pile.h"

card_pile::card_pile(board * boardP) {
	my_board = boardP;
	pile = new vector<card*>;
}

card_pile::card_pile(board * boardP, card * cardP, int ammount) {
	my_board = boardP;
	pile = new vector<card*>;
	add(cardP, ammount);
}

bool card_pile::empty() {
	return pile->empty();
}

int card_pile::size() {
	return pile->size();
}

card * card_pile::pop() {
	card * c = nullptr;
	if (size() > 0) {
		c = pile->back();
		pile->pop_back();
	}
	return c;
}

card * card_pile::remove(int position) {
	card * c = pile->at(position);
	pile->erase(pile->begin() + position);
	return c;
}

card * card_pile::add(card * cardP, int ammount) {
	if (cardP != nullptr && ammount > 0) {
		for (int i = 1; i < ammount; i++) {
			card * copy = new card(cardP);
			pile->push_back(copy);
		}
		pile->push_back(cardP);
	}
	return cardP;
}
		
void card_pile::shuffle() {
    std::shuffle(pile->begin(), pile->end(), my_board->gen);
}

card * card_pile::at(int position) {
	return pile->at(position);
}

card_pile * card_pile::filter_type(string type) {
	vector<card*> * ret_pile = new vector<card*>(*pile);
	int i = 0;
	while (i < ret_pile->size()) {
		if (my_board->coll->cards->at(ret_pile->at(i)->id)->types.count(type) == 0) {
			ret_pile->erase(ret_pile->begin() + i);
		} else {
			i++;
		}
	}
	card_pile * ret = new card_pile(my_board);
	ret->pile = ret_pile;
	return ret;
}

string card_pile::to_string() {
	string ret;
	if (!empty()) {
		int size1 = size() - 1;
		for (int i = 0; i < size1; i++) {
			ret += my_board->coll->cards->at(pile->at(i)->id)->name + ", ";
		}
		ret += my_board->coll->cards->at(pile->at(size1)->id)->name;
	}
	return ret + "\n";
}

string card_pile::count_string() {
	string ret;
	if (!empty()) {
		map<int,int> * count = new map<int,int>;
		int sizeInt = size();
		for (int i = 0; i < sizeInt; i++) {
			if (count->find(pile->at(i)->id) == count->end()) {
				count->emplace(pile->at(i)->id,1);
			} else {
				count->at(pile->at(i)->id)++;
			}
		}
		for (pair<int,int> intint : *count) {
			ret += std::to_string(intint.second) + " " + my_board->coll->cards->at(intint.first)->name + ", ";
		}
		ret.pop_back();
		ret.pop_back();
	}
	return ret + "\n";	
}

#endif