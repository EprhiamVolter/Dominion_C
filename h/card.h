#ifndef card_h
#define card_h

using namespace std;

class card {
	public:
		int id;
		
		card(int idP);
		card(card * copy);
};

#endif