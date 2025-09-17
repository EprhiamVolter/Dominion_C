#ifndef card_cpp
#define card_cpp

#include "card.h"

card::card(int idP) {
	id = idP;
}

card::card(card * copy) {
	id = copy->id;
}

#endif