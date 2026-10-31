#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <vector>
using namespace std;

class Character{
protected:
    string name;
    int hp;
    int maxHp;
    int block;
    
public:
    Character(string name, int initialHp, int maxHp, int block);
    string getName() const {
        return name; 
    }
    int getHp() const {
        return hp; 
    }
    int getMaxHp() const {
        return maxHp; 
    }
    int getBlock() const {
        return block; 
    }

    void addBlock(int blockValue, vector<string>& logs);
    void takeDamage(int damage, vector<string>& logs);
    void resetBlock();
};

class Player : public Character {
protected:
    int energy;
    int maxEnergy;

public:
    Player(string name, int hp, int maxHp, int block, int energy, int maxEnergy);
  
    int getEnergy() const {
        return energy; 
    }
    int getMaxEnergy() const {
        return maxEnergy; 
    }

    void resetEnergy();
    void gainEnergy(int cardEnergyGained, vector<string>& logs);
    void useEnergy(int cardEnergyCost);
};

class Enemy : public Character {
protected:
    int atk;
    int strength;
    int type;

public:
    int intentIndex;

    Enemy(string name, int hp, int maxHp, int block, int atk, int strength, int type);
        
    int getAtk() const {
        return atk; 
    }
    int getType() const {
        return type;
    }
    int getIntentIndex() const {
        return intentIndex;
    }

    void strengthen(vector<string>& logs);
    void attack(Player& p, vector<string>& logs);
    void ultimateAttack(Player& p, vector<string>& logs);
    void changeIntent(Player& p, vector<string>& logs);
};

#endif