#ifndef card_data_cpp
#define card_data_cpp

#include "card_data.h"

card_data::card_data(int idP, string nameP, string card_setP, int costP, set<string> typesP,
		void(*playP)(player * playing), int (*vpP)(player * playing), string textP, string react_toP, bool (*reactP)(player * playing, int pos, int code)) {
	id = idP;
	name = nameP;
	card_set = card_setP;
	cost = costP;
	types = typesP;
	play = playP;
	vp = vpP;
	text = textP;
	react_to = react_toP;
	react = reactP;
}

string card_data::to_string() {
	string ret = "Id " + std::to_string(id) + ", Name " + name + ", Set " + card_set + ", Cost " + std::to_string(cost) + ", Types ";
	for (string type : types) {
		ret +=  type + " ";
	}
	ret.pop_back();
	return ret + "\nText " + text + "\n";
}

#endif