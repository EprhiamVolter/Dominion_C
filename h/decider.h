#ifndef decider_h
#define decider_h

#include <iostream>
#include <vector>

using namespace std;

class player;
class card;

class decider {
	public:
		player * playing;
		bool human_player;
		
		virtual int play() = 0;
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) = 0;
		virtual bool may(string message, int id) = 0;
		
};

class human : public decider {
	public:
		int mode;
	
		human(player * playingP, int modeP = 1);
		virtual int play();
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id);
		virtual bool may(string message, int id);
};

class AI : public decider {
	public:
		AI(player * playingP);
		virtual int play();
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos,  int min, int max, bool repetitive, string message, int id);
		virtual bool may(string message, int id);
};

#include "player.h"
#include "card.h"

#endif