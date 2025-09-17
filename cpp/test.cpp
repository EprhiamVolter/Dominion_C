#ifndef test_cpp
#define test_cpp

#include "board.h"
#include "collection.h"

#include <assert.h>
#include <algorithm>

void test_card() {
	for (int i = -2; i < 3; i++) {
		card * c = new card(i);
		assert(c != nullptr);
		assert(c->id == i);
		
		card * c2 = new card(c);
		assert(c2 != nullptr);
		assert(c2 != c);
		assert(c2->id == c->id);
	}
}

void seven(player *  playing) {
	throw 7;
}

int eigth(player * playing) {
	return 8;
}

bool eleven(player * playing, int pos, int code) {
	return code == 11;
}

void test_card_data() {
	card_data * c = new card_data(1,"2","3",4,{"5","6"},seven,eigth,"9");
	assert(c != nullptr);
	assert(c->id == 1);
	assert(c->name == "2");
	assert(c->card_set == "3");
	assert(c->cost == 4);
	assert(c->types.size() == 2);
	assert(c->types.count("5") == 1);
	assert(c->types.count("6") == 1);
	assert(c->types == set<string>({"5","6"}));
	assert(c->play != nullptr);
	try {
		c->play(nullptr);
		assert(false);
	} catch (int e) {
		assert(e == 7);
	}
	assert(c->vp != nullptr);
	assert(c->vp(nullptr) == 8);
	assert(c->text == "9");
	assert(c->to_string() == "Id 1, Name 2, Set 3, Cost 4, Types 5 6\nText 9\n");
	assert(c->react_to == "");
	assert(c->react == nullptr);
	
	card_data * c2 = new card_data(1,"2","3",4,{"5","6"},seven,eigth,"9","10",eleven);
	assert(c2 != nullptr);
	assert(c2->id == 1);
	assert(c2->name == "2");
	assert(c2->card_set == "3");
	assert(c2->cost == 4);
	assert(c2->types.size() == 2);
	assert(c2->types.count("5") == 1);
	assert(c2->types.count("6") == 1);
	assert(c2->types == set<string>({"5","6"}));
	assert(c2->play != nullptr);
	try {
		c2->play(nullptr);
		assert(false);
	} catch (int e) {
		assert(e == 7);
	}
	assert(c2->vp != nullptr);
	assert(c2->vp(nullptr) == 8);
	assert(c2->text == "9");
	assert(c2->to_string() == "Id 1, Name 2, Set 3, Cost 4, Types 5 6\nText 9\n");
	assert(c2->react_to == "10");
	assert(c2->react(nullptr,0,11));
	assert(!c2->react(nullptr,11,1));
}

void test_collection() {
	collection * c = new collection(false);
	assert(c != nullptr);
	assert(c->loaded != nullptr);
	assert(c->cards != nullptr);
	assert(c->ids != nullptr);
	assert(c->loaded->empty());
	assert(c->cards->empty());
	assert(c->ids->empty());
	
	card_data * numbers = new card_data(-1,"2","3",4,{"5","6"},seven,eigth,"9");
	c->load_card(numbers);
	assert(c->loaded->size() == 0);
	assert(c->cards->size() == 1);
	assert(c->ids->size() == 1);
	assert(c->ids->at("2") == -1);
	assert(c->cards->at(-1) == numbers);
	
	c->load_sets({"Standard"});
	assert(c->loaded->count("Standard") == 1);
	assert(c->loaded->size() == 1);
	assert(c->cards->size() == 8);
	assert(c->ids->size() == 8);
	assert(c->ids->at("Curse") == 6);
	assert(c->cards->at(1)->name == "Silver");
	
	c->load_sets({"Standard","Base"});
	assert(c->loaded->count("Standard") == 1);
	assert(c->loaded->count("Base") == 1);
	assert(c->loaded->size() == 2);
	assert(c->cards->size() == 34);
	assert(c->ids->size() == 34);
	assert(c->ids->at("Curse") == 6);
	assert(c->cards->at(1)->name == "Silver");
	assert(c->ids->at("Witch") == 31);
	assert(c->cards->at(14)->name == "Gardens");
	
	c->load_all();
	assert(c->loaded->count("Standard") == 1);
	assert(c->loaded->count("Base") == 1);
	assert(c->loaded->size() == 2);
	
	vector<int> king = c->kingdom();
	assert(king.size() == 10);
	for (int i = 0; i < 10; i++) {
		assert(c->cards->at(king.at(i))->card_set == "Base");
		assert(king.at(i) <= 32);
		assert(king.at(i) >= 7);
	}
	sort(king.begin(),king.end());
	assert(king.size() == 10);
	for (int i = 0; i < 9; i++) {
		assert(king.at(i) != king.at(i+1));
	}
	
	collection * c2 = new collection();
	assert(c2->loaded->count("Standard") == 1);
	assert(c2->loaded->count("Base") == 1);
	assert(c2->loaded->size() == 2);
}

void test_card_pile() {
	collection * coll = new collection();
	board * b = new board(coll,0,1,0);
	card_pile * c = new card_pile(b);
	assert(c != nullptr);
	assert(c->pile != nullptr);
	assert(c->pile->empty());
	assert(c->my_board == b);
	assert(c->empty());
	assert(c->size() == 0);
	assert(c->pop() == nullptr);
	assert(c->to_string() == "\n");
	assert(c->count_string() == "\n");
	assert(c->filter_type("")->empty());
	try {
		c->at(0);
		assert(false);
	} catch (out_of_range& e) {
	} catch (exception& e) {
		cout << e.what() << endl;
		assert(false);
	}
	try {
		c->remove(0);
		assert(false);
	} catch (out_of_range& e) {
	} catch (exception& e) {
		cout << e.what() << endl;
		assert(false);
	}
	
	c->shuffle();
	assert(c->empty());
	
	card * gold = new card(2);
	assert(c->add(gold,0)->id == 2);
	assert(c->empty());
	
	assert(c->add(gold)->id == 2);
	assert(c->size() == 1);
	assert(c->pop()->id == 2);
	assert(c->empty());
	
	assert(c->add(gold, 5)->id == 2);
	assert(c->size() == 5);
	assert(c->pop()->id == 2);
	assert(c->size() == 4);
	assert(c->at(2)->id == 2);
	assert(c->remove(2)->id == 2);
	assert(c->size() == 3);
	
	c->shuffle();
	card * curse = new card(6);
	assert(c->add(curse,3)->id == 6);
	assert(c->size() == 6);
	assert(c->pop()->id == 6);
	assert(c->size() == 5);
	assert(c->at(2)->id == 2);
	assert(c->remove(3)->id == 6);
	assert(c->remove(1)->id == 2);
	c->add(gold);
	assert(c->to_string() == "Gold, Gold, Curse, Gold\n");
	assert(c->count_string() == "3 Gold, 1 Curse\n");
	assert(c->filter_type("Action")->to_string() == "\n");
	assert(c->filter_type("Treasure")->count_string() == "3 Gold\n");
}

void test_supply() {
	collection * coll = new collection();
	board * b = new board(coll,0,2,0);
	supplyT * s = b->supply;
	assert(s != nullptr);
	assert(s->my_board == b);
	assert(s->empty_piles == 0);
	assert(s->has_buy_for(0)->size() > 1);
	assert(s->has_buy_for(-1)->empty());
	assert(s->piles != nullptr);
	assert(s->piles->size() > 6);
	assert(s->pile_size(0) == 46);
	assert(s->pile_size(1) == 40);
	assert(s->pile_size(2) == 30);
	assert(s->pile_size(3) == 8);
	assert(s->pile_size(4) == 8);
	assert(s->pile_size(5) == 8);
	assert(s->pile_size(6) == 10);
	assert(s->to_string().substr(0,70) == "46 Copper, 40 Silver, 30 Gold, 8 Estate, 8 Duchy, 8 Province, 10 Curse");
	for (int i = 0; i < 7; i++) {
		assert(!b->last_round);
		assert(s->pop(3)->id == 3);
		assert(s->pop(4)->id == 4);
		assert(s->pop(5)->id == 5);
	}
	assert(s->pile_size(4) == 1);
	assert(s->empty_piles == 0);
	assert(!b->last_round);
	assert(s->pop(4)->id == 4);
	assert(s->empty_piles == 1);
	assert(!b->last_round);
	assert(s->pop(5)->id == 5);
	assert(s->empty_piles == 2);
	assert(b->last_round);
	b->last_round = false;
	assert(s->pop(3)->id == 3);
	assert(s->empty_piles == 3);
	assert(b->last_round);
	assert(s->pop(5) == nullptr);
	
	assert(s->piles->size() == 17);
	int king_count = 0;
	int last;
	int left;
	for (int i = 7; i <= 32; i++) {
		if(s->piles->count(i) == 1) {
			king_count++;
			assert(s->pop(i) != nullptr);
			assert(s->pop(i)->id == i);
			left = coll->cards->at(i)->types.count("Victory") == 1 ? 6 : 8;
			assert(s->pile_size(i) == left);
			last = i;
		}
	}
	assert(king_count == 10);
	for (int i = 0; i < left; i++) {
		assert(s->pop(last)->id == last);
	}
	assert(s->pop(last) == nullptr);
	assert(s->empty_piles == 4);
	
	board * b2 = new board(coll,3,0,0);
	supplyT * s2 = b2->supply;
	assert(s2 != nullptr);
	assert(s2->my_board == b2);
	assert(s2->empty_piles == 0);
	assert(s2->has_buy_for(0)->size() > 1);
	assert(s2->has_buy_for(-1)->empty());
	assert(s2->piles->size() > 6);
	assert(s2->pile_size(0) == 39);
	assert(s2->pile_size(1) == 40);
	assert(s2->pile_size(2) == 30);
	assert(s2->pile_size(3) == 12);
	assert(s2->pile_size(4) == 12);
	assert(s2->pile_size(5) == 12);
	assert(s2->pile_size(6) == 20);
	assert(s2->to_string().substr(0,73) == "39 Copper, 40 Silver, 30 Gold, 12 Estate, 12 Duchy, 12 Province, 20 Curse");
	
	
	board * b3 = new board(coll,0,4,0,false,true,{7});
	supplyT * s3 = b3->supply;
	assert(s3 != nullptr);
	assert(s3->pile_size(0) == 32);
	assert(s3->pile_size(3) == 12);
	assert(s3->pile_size(6) == 30);
	assert(s3->piles->size() == 8);
}

void test_player() {
	collection * coll = new collection();
	board * b = new board(coll,0,2,0);
	player * p = b->players->at(1);
	assert(p != nullptr);
	assert(p->my_board == b);
	assert(p->id == 1);
	assert(p->decide != nullptr);
	assert(!p->decide->human_player);
	assert(dynamic_cast<AI*>(p->decide) != nullptr);
	assert(p->deck != nullptr);
	assert(p->deck->size() == 5);
	assert(p->hand != nullptr);
	assert(p->hand->size() == 5);
	assert(p->discard != nullptr);
	assert(p->discard->size() == 0);
	assert(p->played != nullptr);
	assert(p->played->size() == 0);
	assert(p->actions == 1);
	assert(p->coins == 0);
	assert(p->buys == 1);
	assert(p->merchant_token == 0);
	assert(p->vp() == 3);
	card_pile * had = p->has();
	assert(had->size() == 10);
	assert(had->count_string() == "7 Copper, 3 Estate\n");
	
	assert(p->gain(2));
	assert(p->gain(2));
	assert(p->discard->to_string() == "Gold, Gold\n");
	p->draw();
	assert(p->deck->size() == 4);
	assert(p->hand->size() == 6);
	p->draw(5);
	assert(p->deck->to_string() == "Gold\n");
	assert(p->hand->size() == 11);
	assert(p->discard->empty());
	
	assert(p->play_from_hand(-1) == -1);
	assert(p->play_from_hand(11) == -1);
	assert(p->play_from_hand(10) == -2);
	b->phase = Treasure;
	assert(p->play_by_id(1) == -4);
	assert(p->play_from_hand(10) == 2);
	assert(p->coins == 3);
	assert(p->hand->size() == 10);
	assert(p->played->to_string() == "Gold\n");
	
	assert(p->buy(0) == -2);
	b->phase = Buy;
	assert(p->buy(5) == -4);
	b->supply->piles->at(1) = new card_pile(b);
	assert(p->buy(1) == -3);
	assert(p->buy(3) == 3);
	assert(p->buys == 0);
	assert(p->buy(6) == -1);
	assert(p->discard->to_string() == "Estate\n");
	assert(p->coins == 1);
	assert(p->vp() == 4);
	assert(p->has()->count_string() == "7 Copper, 2 Gold, 4 Estate\n");
	
	p->hand->add(new card(27), 2);
	assert(p->play_by_id(27) == -2);
	b->phase = Action;
	assert(p->play_by_id(27) == 27);
	assert(p->actions == 0);
	assert(p->play_by_id(27) == -3);
	assert(p->deck->empty());
	assert(p->discard->empty());
	assert(p->hand->count_string() == "7 Copper, 1 Gold, 4 Estate, 1 Smithy\n");
	assert(p->played->to_string() == "Gold, Smithy\n");
	assert(p->discard_card(new card(31))->id == 31);
	assert(p->gain(2));
	assert(p->gain_to_hand(2));
	assert(p->discard->to_string() == "Witch, Gold\n");
	assert(p->hand->count_string() == "7 Copper, 2 Gold, 4 Estate, 1 Smithy\n");
	assert(p->attack(31));
	assert(p->deck->add(new card(4))->id == 4);
	assert(p->top_deck()->id == 4);
	p->merchant_token = 2;
	p->cleanup();
	assert(p->actions == 1);
	assert(p->coins == 0);
	assert(p->buys == 1);
	assert(p->merchant_token == 0);
	assert(p->vp() == 4);
	assert(p->deck->size() == 13);
	assert(p->hand->size() == 5);
	assert(p->discard->size() == 0);
	assert(p->played->size() == 0);
	p->merchant_token = 5;
	p->play(new card(coll->ids->at("Silver")));
	assert(p->played->to_string() == "Silver\n");
	assert(p->coins == 7);
	
	board * b2 = new board(coll,3,0,0);
	player * p2 = b2->players->at(2);
	assert(p2 != nullptr);
	assert(p2->my_board == b2);
	assert(p2->id == 2);
	assert(p2->decide != nullptr);
	assert(p2->decide->human_player);
	human* h = dynamic_cast<human*>(p2->decide);
	assert(h != nullptr);
	assert(h->mode == 1);
	
	board * b3 = new board(coll,0,0,2);
	player * p3 = b3->players->at(0);
	assert(p3 != nullptr);
	assert(p3->my_board == b3);
	assert(p3->id == 0);
	assert(p3->decide != nullptr);
	assert(p3->decide->human_player);
	human* h3 = dynamic_cast<human*>(p3->decide);
	assert(h3 != nullptr);
	assert(h3->mode == 0);
	
	player * p4 = new player(b2,3);
	assert(p4 != nullptr);
	assert(p4->my_board == b2);
	assert(p4->id == 3);
	assert(p4->decide != nullptr);
	assert(p4->decide->human_player);
	human* h4 = dynamic_cast<human*>(p4->decide);
	assert(h4 != nullptr);
	assert(h4->mode == 1);
	
	player * p5 = new player(b3,5,0);
	assert(p5 != nullptr);
	assert(p5->my_board == b3);
	assert(p5->id == 5);
	assert(p5->decide != nullptr);
	assert(p5->decide->human_player);
	human* h5 = dynamic_cast<human*>(p5->decide);
	assert(h5 != nullptr);
	assert(h5->mode == 0);
}

/*
class player {
	public:
		void play(card * cardP, bool add_to_played = true);
};
*/

void test_mt19937(mt19937 gen) {
	mt19937 no_seed;
	for (int i = 0; i < 5; i++) {
		if (gen() != no_seed()) return;
	}
	assert(false);
}

void test_board() {
	collection * c = new collection();
	board * b = new board(c,0,2);
	assert(b != nullptr);
	assert(b->coll == c);
	test_mt19937(b->gen);
	assert(b->round == 1);
	assert(!b->last_round);
	assert(b->turn == 0);
	assert(b->phase == Action);
	assert(b->player_count == 2);
	assert(b->supply != nullptr);
	assert(b->trash != nullptr);
	assert(b->trash->empty());
	assert(b->players != nullptr);
	assert(b->players->size() == 2);
	assert(b->log != nullptr);
	assert(b->log_tab == 3);
	assert(!b->print_log);
	assert(b->to_string_log().substr(0,115) == "Game Setup\n\tSupply\n\t\tBasic - 46 Copper, 40 Silver, 30 Gold, 8 Estates, 8 Duchies, 8 Provinces, 10 Curses\n\t\tKingdom ");
	assert(b->log->size() == 16);
	assert(b->log->at(4).report == "\tPlayer 0 AI\n");
    assert(b->log->at(5).report == "\t\tStarts with 7 Copper, 3 Estate\n");
    assert(b->log->at(6).report == "\t\tShuffles their deck\n");
    assert(b->log->at(7).report == "\t\tDraws 5 Cards\n");
	assert(b->log->at(8).report == "\tPlayer 1 AI\n");
	for (int i = 0; i < 12; i++) {
		assert(b->log->at(i).round == 0);
	}
	assert(!b->has_play());
	assert(b->players->at(b->turn)->decide->play() == -1);
	assert(b->phase == Treasure);
	assert(b->has_play());
	assert(b->players->at(b->turn)->decide->play() == 0);
	assert(b->end_phase() == Buy);
	assert(b->has_play());
	for (int i = 12; i < 18; i++) {
		assert(b->log->at(i).round == 1);
	}
	assert(b->log->size() == 19);
	assert(b->to_string_log(1) == "\nRound 1\n\tPlayer 0 AI\n\t\tAction Phase\n\t\tTreasure Phase\n\t\t\tPlays Copper\n\t\tBuy Phase\n");
	b->add_log("Testing\n");
	b->reveal(new card(b->coll->ids->at("Silver")),1);
	assert(b->trash_card(new card(b->coll->ids->at("Estate")),-2)->id == b->coll->ids->at("Estate"));
	string log1 = "\nRound 1\n\tPlayer 0 AI\n\t\tAction Phase\n\t\tTreasure Phase\n\t\t\tPlays Copper\n\t\tBuy Phase\n";
	log1 += "\t\t\tTesting\n\t\t\tPlayer 1 reveals a Silver\n\t\t\tEstate trashed from Supply\n";
	assert(b->to_string_log(1) == log1);
	assert(b->trash->to_string() == "Estate\n");
	assert(b->to_string_public().substr(0,74) == "Players 2, Round 1, Turn Player 0, Buy Phase\nTrash\tEstate\nSupply\t46 Copper");
	assert(b->end_card() == "\nPlaces  Winner  Player 1 with 3 Victory Points\n\t\tPlayer 0 with 3 Victory Points\n\n");
	assert(b->players->at(0)->played->to_string() == "Copper\n");
	assert(b->players->at(0)->hand->size() == 4);
	assert(b->players->at(0)->deck->size() == 5);
	assert(b->players->at(0)->discard->size() == 0);
	assert(b->end_phase() == Action);
	assert(b->players->at(0)->played->size() == 0);
	assert(b->players->at(0)->hand->size() == 5);
	assert(b->players->at(0)->deck->size() == 0);
	assert(b->players->at(0)->discard->size() == 5);
	assert(b->turn == 1);
	assert(b->end_phase() == Treasure);
	assert(b->end_phase() == Buy);
	assert(b->end_phase() == Action);
	assert(b->turn == 0);
	assert(b->round == 2);
	b->last_round = true;
	assert(b->end_phase() == Treasure);
	assert(b->end_phase() == Buy);
	assert(b->end_phase() == Action);
	assert(b->end_phase() == Treasure);
	assert(b->end_phase() == Buy);
	assert(b->end_phase() == Ended);
	
	board * b2 = new board(c);
	assert(b2 != nullptr);
	assert(b2->coll == c);
	test_mt19937(b2->gen);
	assert(b2->player_count == 2);
	
	board * b3 = new board(c,3,2,1);
	assert(b3 != nullptr);
	assert(b3->coll == c);
	test_mt19937(b3->gen);
	assert(b3->player_count == 6);
	
	board * b4 = new board(c,0,0,0);
	assert(b4 != nullptr);
	assert(b4->coll == c);
	test_mt19937(b4->gen);
	assert(b4->player_count == 0);
	
	board * b5 = new board(c,0,1,0);
	assert(b5 != nullptr);
	assert(b5->coll == c);
	test_mt19937(b5->gen);
	assert(b5->player_count == 1);
}

void test_AI() {
	collection * c = new collection();
	board * b = new board(c,0,2);
	player * p = b->players->at(0);
	assert(p != nullptr);
	assert(p->my_board == b);
	assert(p->id == 0);
	assert(p->decide != nullptr);
	assert(!p->decide->human_player);
	assert(dynamic_cast<AI*>(p->decide) != nullptr);
	
	assert(p->decide->play() == -1);
	assert(p->decide->play() == 0);
	assert(p->decide->play() == 0);
	b->phase = Buy;
	int bought = p->decide->play();
	assert(c->cards->count(bought) == 1);
	assert(c->cards->at(bought)->cost > 0);
	assert(p->decide->play() == -1);
	
	assert(p->decide->may("test",-1) == true);
	
	card_pile * from = new card_pile(b, new card(0), 10);
	for (int min = 0; min < 10; min++) {
		for (int max = min; max < 10; max++) {
			vector<int> choosen = p->decide->choose_cards(from->pile,vector<int>(),min,max,false,"test",-1);
			int size = choosen.size();
			assert(size <= max);
			assert(size >= min);
			sort(choosen.begin(), choosen.end());
			for (int i = 0; i < size-1; i++) {
				assert(choosen.at(i) != choosen.at(i+1));
			}
		}
	}
	vector<int> pos = {3,5};
	for (int i = 0; i < 10; i++) {
		vector<int> choosen = p->decide->choose_cards(from->pile,pos,i,i,true,"test",-1);
		int size = choosen.size();
		assert(size == i);
		for (int j = 0; j < size-1; j++) {
			assert(choosen.at(j) == 3 || choosen.at(j) == 5);
		}
	}
	
}

class test_decider : public decider {
	public:
		int choices;
		vector<vector<card*>*> froms;
		vector<int> froms_size;
		vector<vector<int>> poss;
		vector<int> mins;
		vector<int> maxs;
		vector<bool> repetitives;
		vector<string> messages;
		vector<int> ids;
		vector<string> may_mess;
		vector<int> may_ids;
		
		test_decider(player * playingP) {
			choices = 0;
			playing = playingP;
			human_player = false;
		}
		
		virtual int play() {
			choices++;
			return 0;
		}
		
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) {
			choices++;
			froms.push_back(from);
			froms_size.push_back(from->size());
			poss.push_back(pos);
			mins.push_back(min);
			maxs.push_back(max);
			repetitives.push_back(repetitive);
			messages.push_back(message);
			ids.push_back(id);
			if (pos.size() == 0) {
				for (int i = 0; i < from->size(); i++) {
					pos.push_back(i);
				}
			}
			vector<int> ret;
			for (int i = 0; i < max; i++) {
				ret.push_back(pos.at(i));
			}
			return ret;
		}
		
		virtual bool may(string message, int id) {
			choices++;
			may_mess.push_back(message);
			may_ids.push_back(id);
			return true;
		}
		
		void assert_cards_at(int at, vector<card *> * from, int from_size, vector<int> pos, int min, int max, bool repetitive, string message, int id) {
			if (from != nullptr) {
				assert(froms.at(at) == from);
			}
			assert(froms_size.at(at) == from_size);
			assert(poss.at(at) == pos);
			assert(mins.at(at) == min);
			assert(maxs.at(at) == max);
			assert(repetitives.at(at) == repetitive);
			assert(messages.at(at) == message);
			assert(ids.at(at) == id);
		}
		
		void assert_may_at(int at, string message, int id) {
			assert(may_mess.at(at) == message);
			assert(may_ids.at(at) == id);
		}
		
		void clear() {
			choices = 0;
			froms.clear();
			froms_size.clear();
			poss.clear();
			mins.clear();
			maxs.clear();
			repetitives.clear();
			messages.clear();
			ids.clear();
			may_mess.clear();
			may_ids.clear();
		}
};
	
void test_kingdom(int feedback) {
	collection * c = new collection();
	map<int,card_data*> * datas = c->cards;
	map<string, int> * ids = c->ids;
	board * b = new board(c,0,4,0,false,true,{});
	player * p = b->players->at(0);
	test_decider * d = new test_decider(p);
	p->decide = d;
	player * p1 = b->players->at(1);
	test_decider * d1 = new test_decider(p1);
	p1->decide = d1;
	player * p2 = b->players->at(2);
	test_decider * d2 = new test_decider(p2);
	p2->decide = d2;
	player * p3 = b->players->at(3);
	test_decider * d3 = new test_decider(p3);
	p3->decide = d3;
	
	if (feedback > 2) cout << "Test Artisan\n";
	assert(p->deck->size() == 5);
	p->play(new card(7),false);
	assert(p->deck->size() == 6);
	assert(p->has()->count_string() == "8 Copper, 3 Estate\n");
	assert(d->choices == 2);
	d->assert_cards_at(0,nullptr,5,{},1,1,true,"Artisan: Gain a card to your hand costing up to 5 Coins. Put a card from your hand onto your deck.",7);
	d->assert_cards_at(1,p->hand->pile,6,{},1,1,false,"Artisan: Put a card from your hand onto your deck.",7);
	d->clear();
	
	if (feedback > 2) cout << "Test Bandit\n";
	p1->discard = p1->deck;
	p1->deck = new card_pile(b);
	p2->deck->add(new card(1));
	p3->deck->add(new card(2),2);
	assert(p->discard->empty());
	assert(p1->deck->size() == 0);
	assert(p1->discard->size() == 5);
	assert(p2->deck->size() == 6);
	assert(p2->discard->size() == 0);
	assert(p3->deck->size() == 7);
	assert(p3->discard->size() == 0);
	p->play(new card(8),false);
	assert(d->choices == 0);
	assert(d1->choices == 0);
	assert(d2->choices == 1);
	d2->assert_cards_at(0,nullptr,1,{},1,1,false,"Bandit: Trash a revealed Treasure other than Copper, and discard the rest.",8);
	d2->clear();
	assert(d3->choices == 1);
	d3->assert_cards_at(0,nullptr,2,{},1,1,false,"Bandit: Trash a revealed Treasure other than Copper, and discard the rest.",8);
	d3->clear();
	assert(p->discard->to_string() == "Gold\n");
	assert(p1->deck->size() == 3);
	assert(p1->discard->size() == 2);
	assert(p2->deck->size() == 4);
	assert(p2->discard->size() == 1);
	assert(p3->deck->size() == 5);
	assert(p3->discard->to_string() == "Gold\n");
	assert(b->trash->to_string() == "Silver, Gold\n");
	
	if (feedback > 2) cout << "Test Bureaucrat\n";
	p->deck->pile->clear();
	p1->hand->pile->clear();
	p2->hand->pile->clear();
	p2->hand->add(new card(6),4);
	p3->hand->pile->clear();
	p3->hand->add(new card(3),2);
	p3->hand->add(new card(2),1);
	p3->hand->add(new card(4),1);
	assert(p->deck->empty());
	assert(p1->hand->empty());
	assert(p2->hand->size() == 4);
	assert(p3->hand->size() == 4);
	assert(p3->deck->size() == 5);
	p->play(new card(9),false);
	assert(d->choices == 0);
	assert(d1->choices == 0);
	assert(d2->choices == 0);
	assert(d3->choices == 1);
	d3->assert_cards_at(0,p3->hand->pile,4,{0,1,3},1,1,false,"Bureaucrat: reveal a Victory card from your hand and put it onto your deck.",9);
	d3->clear();
	assert(p3->hand->size() == 3);
	assert(p3->deck->size() == 6);
	assert(p3->top_deck()->id == 3);
	
	
	if (feedback > 2) cout << "Test Cellar\n";
	assert(p->actions == 1);
	assert(p->deck->size() == 1);
	assert(p->hand->size() == 5);
	assert(p->discard->size() == 1);
	p->play(new card(10),false);
	assert(d->choices == 1);
	d->assert_cards_at(0,p->hand->pile,5,{},0,5,false,"Cellar: Discard any number of cards. +1 Card per card discarded.",10);
	d->clear();
	assert(p->actions == 2);
	assert(p->deck->size() == 2);
	assert(p->hand->size() == 5);
	assert(p->discard->size() == 0);
	
	if (feedback > 2) cout << "Test Chapel\n";
	assert(p->deck->size() == 2);
	assert(p->hand->size() == 5);
	assert(p->discard->size() == 0);
	assert(b->trash->size() == 2);
	p->play(new card(11),false);
	assert(d->choices == 1);
	d->assert_cards_at(0,p->hand->pile,5,{},0,4,false,"Chapel: Trash up to 4 cards from your hand.",11);
	d->clear();
	assert(p->actions == 2);
	assert(p->deck->size() == 2);
	assert(p->hand->size() == 1);
	assert(p->discard->size() == 0);
	assert(b->trash->size() == 6);
	
	if (feedback > 2) cout << "Test Council_Room\n";
	assert(p->buys == 1);
	assert(p->deck->size() == 2);
	assert(p->hand->size() == 1);
	assert(p1->deck->size() == 3);
	assert(p1->hand->size() == 0);
	assert(p2->deck->size() == 4);
	assert(p2->hand->size() == 4);
	assert(p3->deck->size() == 5);
	assert(p3->hand->size() == 3);
	p->play(new card(12),false);
	assert(d->choices == 0);
	assert(d1->choices == 0);
	assert(d2->choices == 0);
	assert(d3->choices == 0);
	assert(p->deck->size() == 0);
	assert(p->hand->size() == 3);
	assert(p1->deck->size() == 2);
	assert(p1->hand->size() == 1);
	assert(p2->deck->size() == 3);
	assert(p2->hand->size() == 5);
	assert(p3->deck->size() == 4);
	assert(p3->hand->size() == 4);
	assert(p->buys == 2);
	
	if (feedback > 2) cout << "Test Festival\n";
	assert(p->buys == 2);
	assert(p->coins == 0);
	assert(p->actions == 2);
	p->play(new card(13),false);
	assert(d->choices == 0);
	assert(p->buys == 3);
	assert(p->coins == 2);
	assert(p->actions == 4);
	
	if (feedback > 2) cout << "Test Gardens\n";
	p->hand->pile->clear();
	p->played->add(new card(14),1);
	p->hand->add(new card(0),8);
	assert(p->vp() == 0);
	p->discard->add(new card(1),1);
	assert(p->vp() == 1);
	p->deck->add(new card(2),29);
	assert(p->vp() == 3);
	p->hand->add(new card(14),1);
	assert(p->vp() == 8);
	p->deck->pile->clear();
	assert(p->vp() == 2);
	
	if (feedback > 2) cout << "Test Harbinger\n";
	p->deck->add(new card(0),3);
	assert(p->actions == 4);
	assert(p->hand->size() == 9);
	assert(p->deck->size() == 3);
	assert(p->discard->size() == 1);
	p->play(new card(15),false);
	assert(d->choices == 1);
	d->assert_cards_at(0,p->discard->pile,1,{},0,1,false,"Harbinger: Look through your discard pile. You may put a card from it onto your deck.",15);
	d->clear();
	assert(p->hand->size() == 10);
	assert(p->deck->size() == 3);
	assert(p->discard->size() == 0);
	p->play(new card(15),false);
	assert(d->choices == 0);
	assert(p->actions == 6);
	assert(p->hand->size() == 11);
	assert(p->deck->size() == 2);
	assert(p->discard->size() == 0);
	
	if (feedback > 2) cout << "Test Laboratory\n";
	assert(p->actions == 6);
	assert(p->hand->size() == 11);
	assert(p->deck->size() == 2);
	p->play(new card(16),false);
	assert(d->choices == 0);
	assert(p->actions == 7);
	assert(p->hand->size() == 13);
	assert(p->deck->size() == 0);
	
	if (feedback > 2) cout << "Test Library\n";
	p->deck->add(new card(0),2);
	p->deck->add(new card(17),1);
	p->deck->add(new card(0),2);
	p->deck->add(new card(17),1);
	assert(p->deck->size() == 6);
	assert(p->hand->size() == 13);
	p->play(new card(17),false);
	assert(d->choices == 0);
	assert(p->deck->size() == 6);
	assert(p->hand->size() == 13);
	assert(p->discard->size() == 0);
	p->hand->pile->clear();
	p->hand->add(new card(0),2);
	p->play(new card(17),false);
	assert(d->choices == 2);
	for (int i = 0; i < 2; i++) {
		d->assert_may_at(i,"Library: skip Library or decline.\n",17);
	}
	d->clear();
	assert(p->deck->size() == 0);
	assert(p->hand->size() == 6);
	assert(p->discard->size() == 2);
	
	if (feedback > 2) cout << "Test Market\n";
	assert(p->actions == 7);
	assert(p->coins == 2);
	assert(p->buys == 3);
	assert(p->deck->size() == 0);
	assert(p->hand->size() == 6);
	assert(p->discard->size() == 2);
	p->play(new card(18),false);
	assert(d->choices == 0);
	assert(p->actions == 8);
	assert(p->coins == 3);
	assert(p->buys == 4);
	assert(p->deck->size() == 1);
	assert(p->hand->size() == 7);
	assert(p->discard->size() == 0);
	
	if (feedback > 2) cout << "Test Merchant\n";
	assert(p->actions == 8);
	assert(p->coins == 3);
	assert(p->deck->size() == 1);
	assert(p->hand->size() == 7);
	assert(p->merchant_token == 0);
	p->play(new card(19),false);
	assert(p->actions == 9);
	assert(p->coins == 3);
	assert(p->deck->size() == 0);
	assert(p->hand->size() == 8);
	assert(p->merchant_token == 1);
	p->play(new card(19),false);
	assert(p->merchant_token == 2);
	p->play(new card(1),false);
	assert(p->merchant_token == 0);
	assert(p->coins == 7);
	p->play(new card(1),false);
	assert(p->coins == 9);
	p->play(new card(19),false);
	assert(p->merchant_token == 1);
	p->cleanup();
	assert(p->merchant_token == 0);
	p->play(new card(1),false);
	assert(p->coins == 2);
	
	if (feedback > 2) cout << "Test Militia\n";
	assert(p1->discard->size() == 2);
	assert(p1->hand->size() == 1);
	assert(p2->discard->size() == 1);
	assert(p2->hand->size() == 5);
	assert(p3->discard->size() == 1);
	assert(p3->hand->size() == 4);
	assert(p->coins == 2);
	p->play(new card(20),false);
	assert(d->choices == 0);
	assert(d1->choices == 0);
	assert(d2->choices == 1);
	d2->assert_cards_at(0,p2->hand->pile,5,{},2,2,false,"Militia: discards down to 3 cards in hand.",20);
	d2->clear();
	assert(d3->choices == 1);
	d3->assert_cards_at(0,p3->hand->pile,4,{},1,1,false,"Militia: discards down to 3 cards in hand.",20);
	d3->clear();
	assert(p1->discard->size() == 2);
	assert(p1->hand->size() == 1);
	assert(p2->discard->size() == 3);
	assert(p2->hand->size() == 3);
	assert(p3->discard->size() == 2);
	assert(p3->hand->size() == 3);
	assert(p->coins == 4);
	
	if (feedback > 2) cout << "Test Mine\n";
	b->trash->pile->clear();
	assert(b->trash->empty());
	p->hand->pile->clear();
	p->hand->add(new card(3),1);
	assert(p->hand->to_string() == "Estate\n");
	p->play(new card(21),false);
	assert(d->choices == 0);
	p->hand->add(new card(0),1);
	assert(p->hand->to_string() == "Estate, Copper\n");
	p->play(new card(21),false);
	assert(d->choices == 2);
	d->assert_cards_at(0,p->hand->pile,2,{1},0,1,false,"Mine: You may trash a Treasure from your hand. Gain a Treasure to your hand costing up to 3 Coins more than it.",21);
	d->assert_cards_at(1,nullptr,2,{},1,1,true,"Mine: Gain a Treasure to your hand costing up to 3 Coins more than Copper (3).",21);
	assert(p->hand->to_string() == "Estate, Copper\n");
	assert(p->hand->remove(1)->id == 0);
	p->hand->add(new card(1),2);
	assert(p->hand->to_string() == "Estate, Silver, Silver\n");
	p->play(new card(21),false);
	assert(d->choices == 4);
	d->assert_cards_at(2,p->hand->pile,3,{1,2},0,1,false,"Mine: You may trash a Treasure from your hand. Gain a Treasure to your hand costing up to 3 Coins more than it.",21);
	d->assert_cards_at(3,nullptr,3,{},1,1,true,"Mine: Gain a Treasure to your hand costing up to 3 Coins more than Silver (6).",21);
	assert(p->hand->to_string() == "Estate, Silver, Copper\n");
	d->clear();
	assert(b->trash->to_string() == "Copper, Silver\n");
	
	if (feedback > 2) cout << "Test Moat\n";
	assert(p->deck->size() == 4);
	assert(p->hand->size() == 3);
	p->play(new card(22),false);
	assert(d->choices == 0);
	assert(p->deck->size() == 2);
	assert(p->hand->size() == 5);
	
	if (feedback > 2) cout << "Test Moat Reaction\n";
	p1->hand->add(new card(3),4);
	p2->hand->add(new card(22),1);
	p3->hand->add(new card(22),2);
	assert(p->discard->empty());
	assert(p->deck->size() == 2);
	assert(p->coins == 4);
	assert(p1->hand->size() == 5);
	assert(p2->hand->size() == 4);
	assert(p3->hand->size() == 5);
	assert(p1->deck->size() == 2);
	assert(p2->deck->size() == 3);
	assert(p3->deck->size() == 4);
	p->play(new card(8),false);
	p->play(new card(20),false);
	p->play(new card(9),false);
	p->play(new card(31),false);
	assert(d->choices == 0);
	assert(p->discard->to_string() == "Gold\n");
	assert(p->deck->size() == 1);
	assert(p->coins == 6);
	assert(p1->hand->size() == 2);
	assert(p2->hand->size() == 4);
	assert(p3->hand->size() == 5);
	assert(p1->deck->size() == 1);
	assert(p2->deck->size() == 3);
	assert(p3->deck->size() == 4);
	assert(d1->choices == 2);
	d1->clear();
	assert(d2->choices == 4);
	assert(d3->choices == 8);
	d2->assert_may_at(0,"Moat: reveal this to be unaffected by Bandit or decline.\n",22);
	d3->assert_may_at(0,"Moat: reveal this to be unaffected by Bandit or decline.\n",22);
	d3->assert_may_at(1,"Moat: reveal this to be unaffected by Bandit or decline.\n",22);
	d2->clear();
	d3->clear();
	
	if (feedback > 2) cout << "Test Moneylender\n";
	assert(p->coins == 6);
	p->hand->pile->clear();
	p->hand->add(new card(0),2);
	p->hand->add(new card(1),1);
	assert(p->hand->size() == 3);
	assert(b->trash->to_string() == "Copper, Silver\n");
	p->play(new card(23),false);
	assert(d->choices == 1);
	d->assert_cards_at(0,p->hand->pile,3,{0,1},0,1,false,"Moneylender: You may trash a Copper from your hand for +3 Coins.",23);
	d->clear();
	assert(p->coins == 9);
	assert(p->hand->to_string() == "Copper, Silver\n");
	assert(b->trash->to_string() == "Copper, Silver, Copper\n");
	
	if (feedback > 2) cout << "Test Poacher\n";
	assert(p->coins == 9);
	assert(p->hand->size() == 2);
	assert(p->deck->size() == 1);
	assert(p->actions == 1);
	p->play(new card(24),false);
	assert(d->choices == 0);
	assert(p->coins == 10);
	assert(p->hand->size() == 3);
	assert(p->deck->size() == 0);
	assert(p->actions == 2);
	b->supply->empty_piles = 3;
	p->play(new card(24),false);
	assert(d->choices == 1);
	d->assert_cards_at(0,p->hand->pile,4,{},3,3,false,"Poacher: Discard a card per empty Supply pile (3).",24);
	d->clear();
	assert(p->hand->size() == 1);
	
	if (feedback > 2) cout << "Test Remodel\n";
	b->trash->pile->clear();
	assert(b->trash->empty());
	p->hand->pile->clear();
	p->hand->add(new card(4),1);
	assert(p->hand->to_string() == "Duchy\n");
	p->play(new card(25),false);
	assert(d->choices == 2);
	d->assert_cards_at(0,p->hand->pile,1,{},1,1,false,"Remodel: Trash a card from your hand. Gain a card costing up to 2 Coins more than it.",25);
	d->assert_cards_at(1,nullptr,6,{},1,1,true,"Remodel: Gain a card costing up to 2 Coins more than Duchy (7).",25);
	assert(p->hand->empty());
	p->play(new card(25),false);
	assert(d->choices == 2);
	d->clear();
	assert(b->trash->to_string() == "Duchy\n");
	
	if (feedback > 2) cout << "Test Sentry\n";
	p->discard->pile->clear();
	p->discard->add(new card(0),3);
	p->deck->add(new card(0));
	assert(p->deck->to_string() == "Copper\n");
	assert(p->discard->to_string() == "Copper, Copper, Copper\n");
	assert(p->hand->empty());
	assert(p->actions == 3);
	p->play(new card(26),false);
	assert(d->choices == 1);
	d->assert_cards_at(0,nullptr,2,{},0,2,false,"Sentry: Trash any number of these. Discard any number of the rest. Put the rest back on top in any order.",26);
	d->clear();
	assert(p->actions == 4);
	assert(p->hand->to_string() == "Copper\n");
	assert(p->deck->to_string() == "Copper\n");
	assert(p->discard->empty());
	assert(b->trash->to_string() == "Duchy, Copper, Copper\n");
	
	if (feedback > 2) cout << "Test Smithy\n";
	p->deck->add(new card(13),2);
	p->deck->add(new card(28),1);
	assert(p->deck->to_string() == "Copper, Festival, Festival, Throne_Room\n");
	assert(p->hand->to_string() == "Copper\n");
	p->play(new card(27),false);
	assert(d->choices == 0);
	assert(p->deck->to_string() == "Copper\n");
	assert(p->hand->to_string() == "Copper, Throne_Room, Festival, Festival\n");
	
	if (feedback > 2) cout << "Test Throne_Room\n";
	assert(p->hand->to_string() == "Copper, Throne_Room, Festival, Festival\n");
	assert(p->played->empty());
	assert(p->actions == 4);
	assert(p->coins == 11);
	assert(p->buys == 1);
	p->play(new card(28),false);
	assert(d->choices == 3);
	d->assert_cards_at(0,p->hand->pile,4,{1,2,3},0,1,false,"Throne_Room: You may play an Action card from your hand twice.",28);
	d->assert_cards_at(1,p->hand->pile,3,{1,2},0,1,false,"Throne_Room: You may play an Action card from your hand twice.",28);
	d->assert_cards_at(2,p->hand->pile,2,{1},0,1,false,"Throne_Room: You may play an Action card from your hand twice.",28);
	d->clear();
	assert(p->hand->to_string() == "Copper\n");
	assert(p->played->to_string() == "Throne_Room, Festival, Festival\n");
	assert(p->actions == 12);
	assert(p->coins == 19);
	assert(p->buys == 5);
	
	if (feedback > 2) cout << "Test Vassal\n";
	p->deck->add(new card(29),1);
	assert(p->deck->to_string() == "Copper, Vassal\n");
	assert(p->played->to_string() == "Throne_Room, Festival, Festival\n");
	assert(p->discard->empty());
	assert(p->coins == 19);
	p->play(new card(29),false);
	assert(d->choices == 1);
	d->assert_may_at(0,"Vassal: Play Vassal or decline\n",29);
	d->clear();
	assert(p->coins == 23);
	assert(p->deck->empty());
	assert(p->discard->to_string() == "Copper\n");
	assert(p->played->to_string() == "Throne_Room, Festival, Festival, Vassal\n");
	
	if (feedback > 2) cout << "Test Village\n";
	assert(p->deck->empty());
	assert(p->hand->to_string() == "Copper\n");
	assert(p->discard->to_string() == "Copper\n");
	assert(p->actions == 12);
	p->play(new card(30),false);
	assert(d->choices == 0);
	assert(p->actions == 14);
	assert(p->deck->empty());
	assert(p->hand->to_string() == "Copper, Copper\n");
	assert(p->discard->empty());
	
	if (feedback > 2) cout << "Test Witch\n";
	p1->discard->pile->clear();
	p2->discard->pile->clear();
	p3->discard->pile->clear();
	p->deck->add(new card(6),3);
	assert(p->deck->to_string() == "Curse, Curse, Curse\n");
	assert(p->hand->to_string() == "Copper, Copper\n");
	p->play(new card(31),false);
	assert(d->choices == 0);
	assert(p->deck->to_string() == "Curse\n");
	assert(p->hand->to_string() == "Copper, Copper, Curse, Curse\n");
	assert(d1->choices == 0);
	d1->clear();
	assert(p1->discard->to_string() == "Curse\n");
	assert(d2->choices == 1);
	d2->assert_may_at(0,"Moat: reveal this to be unaffected by Witch or decline.\n",22);
	d2->clear();
	assert(p2->discard->empty());
	assert(d3->choices == 2);
	d3->assert_may_at(0,"Moat: reveal this to be unaffected by Witch or decline.\n",22);
	d3->assert_may_at(1,"Moat: reveal this to be unaffected by Witch or decline.\n",22);
	d3->clear();
	assert(p3->discard->empty());
	
	if (feedback > 2) cout << "Test Workshop\n";
	assert(b->supply->pile_size(0) == 28);
	assert(p->discard->empty());
	p->play(new card(32),false);
	assert(b->supply->pile_size(0) == 27);
	assert(p->discard->to_string() == "Copper\n");
}

void test_all(int feedback) {
	if (feedback > 1) cout << "Test card\n";
	test_card();
	if (feedback > 1) cout << "Test card_data\n";
	test_card_data();
	if (feedback > 1) cout << "Test collection\n";
	test_collection();
	if (feedback > 1) cout << "Test card_pile\n";
	test_card_pile();
	if (feedback > 1) cout << "Test supply\n";
	test_supply();
	if (feedback > 1) cout << "Test player\n";
	test_player();
	if (feedback > 1) cout << "Test board\n";
	test_board();
	if (feedback > 1) cout << "Test AI\n";
	test_AI();
	if (feedback > 1) cout << "Test kingdom\n";
	test_kingdom(feedback);
	if (feedback > 0) cout << "All tests passed\n";
}

#endif