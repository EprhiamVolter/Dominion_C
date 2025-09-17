#include "board.h"
#include "collection.h"

void test_all(int feedback);

void play(collection * coll, int autopass = 2, int AIs = 0, int manual = 0) {
	board * b = new board(coll, autopass, AIs, manual);
	while (b->phase != Ended) {
		b->players->at(b->turn)->decide->play();
	}
	cout << "\nGame ended\n\nLog\n\n" << b->to_string_log() << "\nFinal board\n\n" << b->to_string_public() << b->end_card();
	
}

int main() {
	
	test_all(2);
	
	collection * c = new collection();
	
	play(c,2,0);
}