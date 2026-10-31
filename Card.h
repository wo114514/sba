#ifndef CARD_H
#define CARD_H

#include <iostream>
using namespace std;

struct Card {
    string name;
    int cost;
    int damage;
    int block;
    int drawNumber;
    int energyGained;
};

extern const Card CARD_DATABASE[];
#endif