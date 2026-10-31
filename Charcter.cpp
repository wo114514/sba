#include "Character.h"
#include <vector>
#include "raylib.h"
#include <iostream>
using namespace std;

// Character
Character::Character(std::string charName, int initialHp, int charMaxHp, int initBlock)
: name(charName), hp(initialHp), maxHp(charMaxHp), block(initBlock) {}

void Character::addBlock(int blockValue, vector<string>& logs){
    logs.push_back(TextFormat("%s gained %d block", name.c_str(), blockValue));
    block += blockValue;
}

void Character::takeDamage(int damage, vector<string>& logs){
    if(block >= damage){
        logs.push_back(TextFormat("%s blocks %d damage", name.c_str(), damage)); 
        block -= damage;
    }
    else{
        damage -= block;
        hp -= damage;
        if (block > 0){
            logs.push_back(TextFormat("%s blocks %d damage", name.c_str(), block)); 
        }
        logs.push_back(TextFormat("%s takes %d damage", name.c_str(), damage));
        block = 0;
    }
    if(hp <= 0){
        logs.push_back(TextFormat("%s is defeated", name.c_str()));
        hp = 0;
    }
}

void Character::resetBlock() {
    block = 0;
}

// Player
Player::Player(string name, int hp, int maxHp, int block, int energy, int maxEnergy)
: Character(name, hp, maxHp, block), energy(energy), maxEnergy(maxEnergy) {}

void Player::resetEnergy() {
    energy = maxEnergy;
}
void Player::gainEnergy(int cardEnergyGained, vector<string>& logs){
    if (cardEnergyGained > 0){
        energy += cardEnergyGained;
        logs.push_back(TextFormat("%s gain %d energy", name.c_str(), cardEnergyGained));
    }
}
void Player::useEnergy(int cardEnergyCost) {
    energy -= cardEnergyCost;
}

// Enemy
Enemy::Enemy(string name, int hp, int maxHp, int block, int atk, int strength, int type)
: Character(name, hp, maxHp, block), atk(atk), strength(strength), type(type), intentIndex(0) {}

void Enemy::strengthen(vector<string>& logs){
    atk += strength;
    logs.push_back(TextFormat("%s strengthens", name.c_str()));
    logs.push_back(TextFormat("%s increasing ATK by %d", name.c_str(), strength));
}
void Enemy::attack(Player& p, vector<string>& logs) {
    if (p.getHp() > 0){
        p.takeDamage(atk, logs); 
    }
}
void Enemy::ultimateAttack(Player& p, vector<string>& logs) {
    int damage = p.getHp()/12+1;
    for (int i = 0; i < 6; i++){
        if (p.getHp() <= 0){
            break;
        }
        p.takeDamage(damage, logs); 
    }
}
void Enemy::changeIntent(Player& p, vector<string>& logs){
    // Nibbit
    if (type == 0){    
        if (intentIndex == 0) {
            p.takeDamage(atk, logs); 
            intentIndex = 1; 
        } 
        else if (intentIndex == 1) {
            p.takeDamage(atk, logs);
            addBlock(atk, logs);
            intentIndex = 2; 
        }
        else {
            strengthen(logs);
            intentIndex = 0; 
        }
    }
    // Anthony
    if (type == 1){
        if (intentIndex == 0) {
            p.takeDamage(atk, logs); 
            intentIndex = 1; 
        } 
        else if (intentIndex == 1) {
            p.takeDamage(atk, logs); 
            addBlock(16, logs);
            intentIndex = 2; 
        }
        else {
            intentIndex = 0; 
        }
    }
    // Effigy
    if (type == 2){
        if (intentIndex == 0) {
            intentIndex = 1; 
        } 
        else if (intentIndex == 1) {
            strengthen(logs);
            intentIndex = 2; 
        }
        else {
            attack(p,logs);
            intentIndex = 2; 
        }
    }
    // Hexagonal Ghost
    if (type == 3){
        if (intentIndex == 0) {
            intentIndex = 1; 
        } 
        else if (intentIndex == 1) {
            ultimateAttack(p,logs);
            intentIndex = 2; 
        }
        else if (intentIndex == 2) {
            attack(p,logs);
            attack(p,logs);
            intentIndex = 3; 
        }
        else if (intentIndex == 3) {
            attack(p,logs);
            intentIndex = 4; 
        }
        else if (intentIndex == 4) {
            addBlock(16, logs);
            intentIndex = 5; 
        }
        else if (intentIndex == 5) {
            strengthen(logs);
            intentIndex = 6; 
        }
        else if (intentIndex == 6) {
            intentIndex = 7; 
        }
        else if (intentIndex == 7) {
            attack(p,logs);
            attack(p,logs);
            attack(p,logs);
            attack(p,logs);
            attack(p,logs);
            attack(p,logs);
            intentIndex = 8; 
        }
        else {
            attack(p,logs);
            intentIndex = 2; 
        }
    }
}     