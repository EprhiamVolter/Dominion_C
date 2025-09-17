#ifndef card_data_h
#define card_data_h

#include <iostream>
#include <set>

using namespace std;

class player;

class card_data {
	public:
		int id;
		string name;
		string card_set;
		int cost;
		set<string> types;
		string text;
		string react_to;
		
		card_data(int idP, string nameP, string card_setP, int costP, set<string> typesP, void (*play)(player * playing), int (*vp)(player * playing), string textP,
		string reac_toP = "", bool (*reactP)(player * playing, int pos, int code) = nullptr);
		void (*play)(player * playing);
		int (*vp)(player * playing);
		bool (*react)(player * playing, int pos, int code);
		string to_string();
};

#include "player.h"

#endif