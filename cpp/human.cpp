#ifndef human_cpp
#define human_cpp

#include "decider.h"

human::human(player * playingP, int modeP) {
	playing = playingP;
	human_player = true;
	mode = modeP;
}

string cmd;
string word;

string getWord() {
    size_t end = cmd.find(' ');
    word = cmd.substr(0, end);
	if (end == string::npos) {
		cmd.clear();
	} else {
		cmd = cmd.substr(end + 1, string::npos);
	}
	return word;
}

bool wordIsNum() {
	return (word.size() > 0) && (word[0] == '-' || (word[0] >= '0' && word[0] <= '9'));
}

bool handle_info(board * b, string message) {
	while (true) {
		try {
			cout << message;
			getline(cin,cmd);
			getWord();
			while (word.empty()) {
				getline(cin,cmd);
				getWord();
			} if (word == "exit" || word == "Exit") {
				b->phase = Ended;
				return false;
			} else if (word == "help" || word == "Help") {
				cout << "\texit : Exits\n" <<
				"Information:\n" <<
				"\thelp : Prints this list of commands\n" << 
				"\trules : Prints the rules of the game\n" <<
				"\tboard : Prints all data of the board visible to the current player\n" << 
				"\thand : Prints all Cards on the current player hand\n" <<
				"\thas : Prints all Cards on the current player hand, deck, discard and play\n"
				"\tlog : Prints the log, including the entire history of the board\n" <<
				"\tlog [from] : Prints the log from round [from] to the end. [from] can be a number or \"last\"\n" <<
				"\tinfo [card] : Prints the data of the Card of name [card]\n" <<
				"Game Actions\n" <<
				"\t[yes|no] : When asked a yes or no question, can answer with yes Yes y Y : no No not Not n N\n" <<
				"\t[pos|card] : When asked to choose from Cards, can select by position or name (leftmost if multiple). -1 or \"none\" to choose nothing if possible\n" <<
				"\tplay [pos|card] : Plays the Card in postition [pos] of hand or the leftmost of name [card]\n" <<
				"\tall : Plays all Treasures in hand\n" <<
				"\tbuy [card] : Buys the card of name [card]\n" <<
				"\tend : Ends the current phase\n";
			} else if (word == "rules" || word == "Rules") {
				cout << "see Dominion\n";
			} else if (word == "board" || word == "Board") {
				cout << endl << b->to_string_for_player(b->turn);
			} else if (word == "hand" || word == "Hand") {
				cout << "Hand\t" << b->players->at(b->turn)->hand->to_string();
			} else if (word == "has" || word == "Has") {
				cout << "Has\t" + b->players->at(b->turn)->has()->count_string();
			} else if (word == "log" || word == "Log") {
				int from = 0;
				if (!getWord().empty()) {
					if (word == "last" || word == "Last") {
						from = b->turn == 0 ? b->round - 1 : b->round;
					} else {
						from = stoi(word);
					}
				}
				cout << endl << b->to_string_log(from);
			} else if (word == "info" || word == "Info") {
				getWord();
				if (b->coll->ids->count(word) == 1) {
					cout << b->coll->cards->at(b->coll->ids->at(word))->to_string();
				} else {
					cout << "\"" << word << "\" not found in the collection\n";
				}
			} else {
				return true;
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
}

int human::play() {
	board * b = playing->my_board;
	while (mode == 1 && !b->has_play()) {
		phases to_end = b->phase;
		b->end_phase();
		cout << phase_name(to_end) << " Phase ended\n";
		if (b->phase == Ended || b->turn != playing->id) {
			return -1;
		}
	}
	string message = "\nRound " + to_string(b->round) + ", Player " + to_string(b->turn) + ", " + phase_name(b->phase) + " Phase. Awaiting input, type \"help\" for commands:\n";
	if (b->phase == Action) {
		message += "Actions " + to_string(playing->actions) + " Hand\t" + playing->hand->to_string();
	} else if (b->phase == Treasure) {
		message += "Hand\t" + playing->hand->to_string();
	} else if (b->phase == Buy) {
		message += "Supply\t" + b->supply->to_string() +
		"Buys " + to_string(playing->buys) + " Coins " + to_string(playing->coins) + "\n";
	}
	while (handle_info(b, message)) {
		try {
			if (word == "play" || word == "Play") {
				getWord();
				if (wordIsNum()) {
					int pos = stoi(word);
					int result = playing->play_from_hand(pos);
					if (result >= 0) {
						cout << b->coll->cards->at(result)->name << " played\n";
						return result;
					} else switch(result) {
						case -1 : cout << pos << " is out of range, hand has " << playing->hand->size() << " Cards\n"; break;
						case -2 : cout << "The Card cannot be played in " + phase_name(b->phase) + " Phase\n"; break;
						case -3 : cout << "0 Actions avaiable\n"; break;
						default : cout << "Unknown error\n";
					}
				} else if (b->coll->ids->count(word) == 1) {
					string name = word;
					int result = playing->play_by_id(b->coll->ids->at(word));
					if (result >= 0) {
						cout << name << " played\n";
						return result;
					} else switch(result) {
						case -2 : cout << "The Card cannot be played in " + phase_name(b->phase) + " Phase\n"; break;
						case -3 : cout << "0 Actions avaiable\n"; break;
						case -4 : cout << name << " is not in the hand\n"; break;
						default : cout << "Unknown error\n";
					}
				} else {
					cout << "\"" << word << "\" not found in the collection\n";
				}
			} else if (word == "all" || word == "All") {
				if (b->phase != Treasure) {
					cout << "Cannot play all Treasures on " + phase_name(b->phase) + " Phase\n";
				} else {
					int i = 0;
					int result = playing->play_from_hand(i);
					while (result != -1) {
						if (result >= 0) {
							cout << b->coll->cards->at(result)->name << " played\n";
						} else {
							i++;
						}
						result = playing->play_from_hand(i);
					}
					return -2; //TODO better manage play all return value
				}
			} else if (word == "buy" || word == "Buy") {
				getWord();
				int result = playing->buy(b->coll->ids->at(word));
				if (result >= 0) {
					cout << word << " bought\n";
					return result;
				} else switch(result) {
					case -1 : cout << "0 Buys avaiable\n"; break;
					case -2 : cout << "Cannot Buy outside the Buy Phase\n"; break;
					case -3 : cout << word << " is not on the supply or its pile its empty\n"; break;
					case -4 : cout << "Unsuficient Coins to Buy " << word << "\n"; break;
					default : cout << "Unknown error\n";
				}
			} else if (word == "end" || word == "End") {
				phases to_end = b->phase;
				b->end_phase();
				cout << phase_name(to_end) << " Phase ended\n";
				return -1;
			} else {
				cout << "\"" << word << "\" command not found. Type \"help\" for a list of commands\n";
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
	return 0;
}

vector<int> human::choose_cards(vector<card*> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) {
	board * b = playing->my_board;
	vector<int> ret;
	int choosed = 0;
	if (pos.size() == 0) {
		for (int i = 0; i < from->size(); i++) {
			pos.push_back(i);
		}
	}
	string options = " Options:\n";
	for (int i = 0; i < pos.size(); i++) {
		options += b->coll->cards->at(from->at(pos.at(i))->id)->name + ", ";
	}
	options.pop_back();
	options.pop_back();
	options += "\n";
	string message2 = "Player " + to_string(playing->id) + " : " + message;
	while (choosed < max && handle_info(b, message2 + options)) {
		try {
			int position = 0;
			if (wordIsNum()) {
				position = stoi(word);
				if (position == -1) {
					if (choosed >= min) {
						break;
					} else {
						cout << "Must choose at least " << min << " Cards. So far " << choosed << endl; 
						continue;
					}
				} else if (position < 0 || position >= pos.size()) {
					cout << position << " out of range " << pos.size() << endl;
					continue;
				}
			} else {
				if (word == "none" || word == "None") {
					if (choosed >= min) {
						break;
					} else {
						cout << "Must choose at least " << min << " Cards. So far " << choosed << endl; 
						continue;
					}
				} else if (b->coll->ids->count(word) == 0) {
					cout << word << " not found in the collection\n";
					continue;
				}
				int id = b->coll->ids->at(word);
				while (position < pos.size() && from->at(pos.at(position))->id != id) {
					position++;
				}
				if (position == pos.size()) {
					cout << word << " not in options\n";
					continue;
				}
			}
			ret.push_back(pos.at(position));
			pos.erase(pos.begin() + position);
			choosed++;
			options = " Options:\n";
			for (int i = 0; i < pos.size(); i++) {
				options += b->coll->cards->at(from->at(pos.at(i))->id)->name + ", ";
			}
			options.pop_back();
			options.pop_back();
			options += "\n";
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
	return ret;
}

bool human::may(string message, int id) {
	string message2 = "Player " + to_string(playing->id) + " : " + message;
	while (true) {
		try {
			handle_info(playing->my_board, message2);
			if (word == "yes" || word == "Yes" || word == "y" || word == "Y") {
				return true;
			} else if (word == "no" || word == "No" || word == "not" || word == "Not" || word == "n" || word == "N") {
				return false;
			} else {
				cout << "\"" << word << "\" not recognized, answer with \"yes\" or \"no\"\n";
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
}

#endif