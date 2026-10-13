#include "raylib.h"
#include <iostream>
#include <vector>
using namespace std;

enum GameState {
    STATE_MENU,
    STATE_PLAYER_TURN,
    STATE_VIEW_PILE,
    STATE_MATH_QUIZ,
    STATE_ENEMY_TURN,
    STATE_GAME_OVER
};

class Character{
protected:
    string name;
    int hp;
    int maxHp;
    int block;
    
public:
    Character(string charName, int initialHp, int charMaxHp, int initBlock)
    : name(charName), hp(initialHp), maxHp(charMaxHp), block(initBlock) {}
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

    void addBlock(int blockValue, vector<string>& logs){
        logs.push_back(TextFormat("%s gained %d block", name.c_str(), blockValue));
        block += blockValue;
    }
    void takeDamage(int damage, vector<string>& logs){
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
    void resetBlock() {
        block = 0;
    }
};

class Player : public Character {
protected:
    int energy;
    int maxEnergy;

public:
    Player(string name, int hp, int maxHp, int block, int energy, int maxEnergy) 
    : Character(name, hp, maxHp, block), energy(energy), maxEnergy(maxEnergy) {}
        
    int getEnergy() const {
        return energy; 
    }
    int getMaxEnergy() const {
        return maxEnergy; 
    }

    void resetEnergy() {
        energy = maxEnergy;
    }
    void gainEnergy(int cardEnergyGained, vector<string>& logs){
        if (cardEnergyGained > 0){
            energy += cardEnergyGained;
            logs.push_back(TextFormat("%s gain %d energy", name.c_str(), cardEnergyGained));
        }
    }
    void useEnergy(int cardEnergyCost) {
        energy -= cardEnergyCost;
    }
};

class Enemy : public Character {
protected:
    int atk;
    int strength;
    int type;

public:
    int intentIndex;

    Enemy(string name, int hp, int maxHp, int block, int atk, int strength, int type) 
        : Character(name, hp, maxHp, block), atk(atk), strength(strength), type(type), intentIndex(0) {}
        
    int getAtk() const {
        return atk; 
    }
    int getType() const {
        return type;
    }
    int getIntentIndex() const {
        return intentIndex;
    }

    void strengthen(vector<string>& logs){
        atk += strength;
        logs.push_back(TextFormat("%s strengthens", name.c_str()));
        logs.push_back(TextFormat("%s increasing ATK by %d", name.c_str(), strength));
    }
    void attack(Player& p, vector<string>& logs) {
        if (p.getHp() > 0){
            p.takeDamage(atk, logs); 
        }
    }
    void ultimateAttack(Player& p, vector<string>& logs) {
        int damage = p.getHp()/12+1;
        for (int i = 0; i < 6; i++){
            if (p.getHp() <= 0){
                break;
            }
            p.takeDamage(damage, logs); 
        }
    }     
    void changeIntent(Player& p, vector<string>& logs){
        if (type == 0){
            // Nibbit
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
};

struct Card {
    string name;
    int cost;
    int damage;
    int block;
    int drawNumber;
    int energyGained;
};

const Card CARD_DATABASE[] = {
    {"Strike", 1, 6, 0, 0, 0},//0
    {"Defend", 1, 0, 5, 0, 0},//1
    {"Bludgeon", 3, 32, 0, 0, 0},//2
    {"IronWave", 1, 5, 5, 0, 0},//36
    {"Ultimate Strike", 1, 11, 0, 0, 0},//4
    {"Ultimate Defend", 1, 0, 11, 0, 0},//5
    {"Dash", 2, 11, 11, 0, 0},//6
    {"Pommel Strike", 1, 9, 0, 1, 0},//7
    {"Shrug It Off", 1, 0, 8, 1, 0},//8
    {"Battle Trance", 0, 0, 0, 2, 0},//9
    {"Seeing Red", 0, 0, 0, 0, 2}//10
};

void shuffleDeck(vector<string>& logs, vector<int>& discardPile, vector<int>& drawPile) {
    if (!discardPile.empty()){
        for (int i = 0; i < discardPile.size(); i++){
            drawPile.push_back(discardPile[i]);
        }
        discardPile.clear();
    }
    if (!drawPile.empty()){
        for (int i = 0; i < drawPile.size(); i++){
            int swapIndex = GetRandomValue(0, drawPile.size() - 1);
            int tempCard = drawPile[i];
            drawPile[i] = drawPile[swapIndex];
            drawPile[swapIndex] = tempCard;
        }
        logs.push_back("Cards reshuffled");
    }
}

void drawCards(vector<string>& logs, vector<int>& drawPile, vector<int>& hand, vector<int>& discardPile, int numToDraw) {
    while (numToDraw > 0) {
        if (drawPile.empty()) {
            shuffleDeck(logs, discardPile, drawPile);
            if (drawPile.empty()) {
                logs.push_back("Not enough cards to draw");
                break; 
            }
        }
        int topCardID = drawPile.back(); 
        if (hand.size() < 7){
            hand.push_back(topCardID);   
        }
        else{
            discardPile.push_back(topCardID);   
            logs.push_back("Hand is full");
        }
        drawPile.pop_back();          
        numToDraw--; 
    }
}

void discardHand(vector<int>& hand, vector<int>& discardPile){
    if (!hand.empty()){
        for (int i = 0; i < hand.size(); i++){
            discardPile.push_back(hand[i]);
        }
        hand.clear();
    }
}

int drawDeck(const vector<int>& deck, int startY, Font font, int playerEnergy, int selectedCardIdx, bool isUpdate) {
    if (deck.empty()) {
        int textWidth = MeasureTextEx(font, "Your hand is empty!", 26, 1).x;
        DrawTextEx(font, "Your hand is empty!", Vector2{(1280.0f - textWidth) / 2.0f, (float)startY + 50}, 26, 1, DARKGRAY);
        return -1;
    }

    const float cardWidth = 140.0f;
    const float cardHeight = 180.0f;
    const float cardSpacing = 16.0f;

    int numCards = deck.size();
    float totalDeckWidth = (numCards * cardWidth) + ((numCards - 1) * cardSpacing);
    float startX = (1280.0f - totalDeckWidth) / 2.0f;

    int clickedIndex = -1;

    for (int i = 0; i < numCards; i++) {
        int cardID = deck[i];
        Card currentCard = CARD_DATABASE[cardID];

        float posX = startX + i * (cardWidth + cardSpacing);
        float currentCardY = (float)startY;

        Rectangle baseCheckRect = { posX, (float)startY, cardWidth, cardHeight };
        bool isHovered = CheckCollisionPointRec(GetMousePosition(), baseCheckRect);

        if (i == selectedCardIdx || isHovered) {
            currentCardY = (float)startY - 40.0f;
        }

        Rectangle cardRect = { posX, currentCardY, cardWidth, cardHeight };

        if (isUpdate && isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clickedIndex = i;
        }

        Color baseColor = Color{ 30, 30, 40, 255 };
        Color borderColor = GRAY; // Change border color

        if (playerEnergy < currentCard.cost) {
            borderColor = RED;
        } else if (isHovered || i == selectedCardIdx) {
            borderColor = GOLD;
        }

        if (isHovered || i == selectedCardIdx) {
            baseColor = Color{ 50, 50, 70, 255 };
        }

        // Draw Cards and border
        DrawRectangleRec(cardRect, baseColor);
        DrawRectangleLinesEx(cardRect, 1, borderColor);

        // Draw energy
        float energyCircleX = posX + 10;
        float energyCircleY = currentCardY;
        DrawCircle(energyCircleX, energyCircleY, 16, ORANGE);
        DrawCircleLines(energyCircleX, energyCircleY, 16, WHITE);
        DrawTextEx(font, TextFormat("%d", currentCard.cost), Vector2{energyCircleX - 7, energyCircleY - 12}, 26, 1, WHITE);

        float textAlignX = posX + 15;
        string cardName = currentCard.name;
        int splitPos = (int)cardName.find_first_of("- ");

        if (splitPos != -1) {
            string firstLine = cardName.substr(0, splitPos);
            string secondLine = cardName.substr(splitPos + 1);
            DrawTextEx(font, firstLine.c_str(), Vector2{textAlignX, currentCardY + 24}, 22, 1, WHITE);
            DrawTextEx(font, secondLine.c_str(), Vector2{textAlignX, currentCardY + 46}, 22, 1, WHITE);
        } else {
            DrawTextEx(font, cardName.c_str(), Vector2{textAlignX, currentCardY + 24}, 22, 1, WHITE);
        }

        // Draw ATK
        if (currentCard.damage > 0) {
            DrawTextEx(font, TextFormat("ATK: %d", currentCard.damage), Vector2{textAlignX, currentCardY + 77}, 22, 1, RED);
        }

        // Draw DEF
        if (currentCard.block > 0) {
            float blockYOffset;
            if (currentCard.damage > 0) {
                blockYOffset = currentCardY + 99.0f;
            } else {
                blockYOffset = currentCardY + 77.0f;
            }
            DrawTextEx(font, TextFormat("DEF: %d", currentCard.block), Vector2{textAlignX, blockYOffset}, 22, 1, BLUE);
        }

        // Draw DRAW
        if (currentCard.drawNumber > 0) {
            float drawYOffset = currentCardY + 77.0f;
            if (currentCard.damage > 0) { drawYOffset += 22.0f; }
            if (currentCard.block > 0)  { drawYOffset += 22.0f; }
            DrawTextEx(font, TextFormat("DRAW: %d", currentCard.drawNumber), Vector2{textAlignX, drawYOffset}, 22, 1, GOLD);
        }

        // Draw NRG
        if (currentCard.energyGained > 0) {
            float energyGainedYOffset = currentCardY + 77.0f;
            if (currentCard.damage > 0)     { energyGainedYOffset += 22.0f; }
            if (currentCard.block > 0)      { energyGainedYOffset += 22.0f; }
            if (currentCard.drawNumber > 0) { energyGainedYOffset += 22.0f; }
            DrawTextEx(font, TextFormat("NRG: +%d", currentCard.energyGained), Vector2{textAlignX, energyGainedYOffset}, 22, 1, ORANGE);
        }

        // Draw Card Index
        DrawTextEx(font, TextFormat("#%d", i + 1), Vector2{posX + cardWidth - 34, currentCardY + cardHeight - 22}, 16, 1, WHITE);
    }

    return clickedIndex;
}

struct QuizInfo {
    string title;
    string options; // Options
    string optA;    // Stores text A
    string optB;    // Stores text B
    string optC;    // Stores text C
    string optD;    // Stores text D
    char correctAns;
};

struct GameButton {
    Rectangle rect;
    string text1;
    string text2;
    string text3;
    bool isHovered;

    // Init button positions and size
    void init(float x, float y, float width, float height, string t1, string t2 = "", string t3 = "") {
        rect = { x, y, width, height };
        text1 = t1;
        text2 = t2;
        text3 = t3;
        isHovered = false;
    }

    // Hover detection
    void update(Vector2 mousePos) {
        isHovered = CheckCollisionPointRec(mousePos, rect);
    }

    // Click detection
    bool isClicked(Vector2 mousePos) {
        return isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    // Render button and border
    void draw(Font font, Color baseColor, Color hoverColor, Color borderColor) {
        update(GetMousePosition());
        Color currentBg;
        
        // Hover Color change
        if (isHovered) {
            currentBg = hoverColor;
        } else {
            currentBg = baseColor;
        }
        
        // Draw rectangle and border lines
        DrawRectangleRec(rect, currentBg);
        DrawRectangleLinesEx(rect, 1, borderColor);

        // Text positioning logic 
        if (text2 == "" && text3 == "") {
            // Centered 
            Vector2 textSize = MeasureTextEx(font, text1.c_str(), 20, 1);
            float tx = rect.x + (rect.width - textSize.x) / 2.0f;
            float ty = rect.y + (rect.height - textSize.y) / 2.0f;
            DrawTextEx(font, text1.c_str(), Vector2{tx, ty}, 20, 1, WHITE);
        } else {
            // Centered when more lines
            DrawTextEx(font, text1.c_str(), Vector2{ rect.x + 18, rect.y + 10 }, 20, 1, WHITE);
            if (text2 != "") DrawTextEx(font, text2.c_str(), Vector2{ rect.x + 18, rect.y + 32 }, 20, 1, WHITE);
            if (text3 != "") DrawTextEx(font, text3.c_str(), Vector2{ rect.x + 18, rect.y + 54 }, 20, 1, WHITE);
        }
    }
};

struct QuizButton {
    Rectangle rect;
    string text;
    //'A', 'B', 'C', or 'D'
    char choice; 
    bool isHovered;
    // Init buttons
    void init(char ch) {
        choice = ch;
        rect = { 0, 0, 0, 0 };
        text = "";
        isHovered = false;
    }
    // Hitbox update
    void update(Vector2 mousePos, const string& optionText, float posX, float posY, int fontSize, Font font) {
        text = optionText;
        int textWidth = MeasureTextEx(font, text.c_str(), fontSize, 1.0f).x; 
        rect = { posX, posY, (float)textWidth + 30, (float)fontSize + 20 };
        isHovered = CheckCollisionPointRec(mousePos, rect);
    }

    // Draws the button box, border, hover colors
    void draw(int fontSize, Font font) {
        Color boxColor;
        Color borderColor;
        Color textColor;

        if (isHovered) {
            boxColor = Color{ 45, 60, 85, 255 };
            borderColor = GOLD;
            textColor = GOLD;
        }
        else {
            boxColor = Color{ 20, 25, 35, 255 };
            borderColor = Color{ 80, 90, 105, 255 };
            textColor = WHITE;
        }

        DrawRectangleRec(rect, boxColor);
        DrawRectangleLinesEx(rect, 2, borderColor); 
        DrawTextEx(font, text.c_str(), Vector2{rect.x + 15, rect.y + 10}, fontSize, 1, textColor);
    }
};

class QuestionGenerator {
public:
    QuestionGenerator() {}

    QuizInfo generateAlgebraQuestion() {
        int A = GetRandomValue(1, 10);
        int B = GetRandomValue(1, 10);
        int correctVal = A + B;
        int wrong1 = A - B;
        int wrong2 = A + B - GetRandomValue(1, 10);
        int wrong3 = A + B + GetRandomValue(1, 10);
        int targetPos = GetRandomValue(1, 4);

        QuizInfo quiz;
        int lengthType = GetRandomValue(1, 3); // 1 = Short, 2 = Medium, 3 = Long

        if (lengthType == 1) {
        // Standard 1-line question
            quiz.title = TextFormat("Simplify: %d y - (-%d y) = ?", A, B);
        } 
        else if (lengthType == 2) {
        // Simulated 2-line question using '\n'
            quiz.title = TextFormat("Advanced Algebra Problem (Turn %d):\nSimplify the basic linear expression: %d y - (-%d y) = ?", GetRandomValue(1, 5), A, B);
        } 
        else {
        // Simulated 3-line question using multiple '\n'
            quiz.title = TextFormat("--- [MISSION CHALLENGE] ---\nGiven that y is a real variable and coefficients are non-zero.\nPlease calculate and simplify: %d y - (-%d y) = ?", A, B);
        }

        if (targetPos == 1) {
            quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", correctVal, wrong1, wrong2, wrong3);
            quiz.optA = TextFormat("A. %dy", correctVal);
            quiz.optB = TextFormat("B. %dy", wrong1);
            quiz.optC = TextFormat("C. %dy", wrong2);
            quiz.optD = TextFormat("D. %dy", wrong3);
            quiz.correctAns = 'A';
        } 
        else if (targetPos == 2) {
            quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, correctVal, wrong2, wrong3);
            quiz.optA = TextFormat("A. %dy", wrong1);
            quiz.optB = TextFormat("B. %dy", correctVal);
            quiz.optC = TextFormat("C. %dy", wrong2);
            quiz.optD = TextFormat("D. %dy", wrong3);
            quiz.correctAns = 'B';
        } 
        else if (targetPos == 3) {
            quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, wrong2, correctVal, wrong3);
            quiz.optA = TextFormat("A. %dy", wrong1);
            quiz.optB = TextFormat("B. %dy", wrong2);
            quiz.optC = TextFormat("C. %dy", correctVal);
            quiz.optD = TextFormat("D. %dy", wrong3);
            quiz.correctAns = 'C';
        } 
        else {
            quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, wrong2, wrong3, correctVal);
            quiz.optA = TextFormat("A. %dy", wrong1);
            quiz.optB = TextFormat("B. %dy", wrong2);
            quiz.optC = TextFormat("C. %dy", wrong3);
            quiz.optD = TextFormat("D. %dy", correctVal);
            quiz.correctAns = 'D';
        }

        return quiz;
    }
};

int main(){
    // Init Screen
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Slay the Math");
    SetTargetFPS(60); 

    // Init Random Seed
    SetRandomSeed((unsigned int)time(NULL)); 

    // Init font
    Font Thin = LoadFont("resources/font/JetBrainsMono-Thin.ttf");  
    Font Regular = LoadFont("resources/font/JetBrainsMono-Regular.ttf");
    Font SemiBold = LoadFont("resources/font/JetBrainsMono-SemiBold.ttf");
    Font Bold = LoadFont("resources/font/JetBrainsMono-Bold.ttf");

    // Init Character
    Player p ("IronClad", 80, 80, 0, 3, 3);
    Enemy e ("Dummy", 114514, 114514, 202266, 234, 20, 0);
    int enemyType = GetRandomValue(0, 3);
    if (enemyType == 0){
        e =  Enemy("Nibbit", 96, 96, 0, 15, 3, 0);
    }
    else if (enemyType == 1){
        e =  Enemy("Anthony", 112, 112, 16, 16, 1, 1);
    }
    else if (enemyType == 2){
        e =  Enemy("Bygone Effigy", 172, 172, 0, 15, 10, 2);
    }
    else{
        e =  Enemy("Hexagonal Ghost", 304, 304, 0, 4, 1, 3);
    }
    
    //InitDeck Info
    /*
    const Card CARD_DATABASE[] = {
        {"Strike", 1, 6, 0, 0, 0},//0
        {"Defend", 1, 0, 5, 0, 0},//1
        {"Bludgeon", 3, 32, 0, 0, 0},//2
        {"IronWave", 1, 5, 5, 0, 0},//36
        {"Ultimate Strike", 1, 11, 0, 0, 0},//4
        {"Ultimate Defend", 1, 0, 11, 0, 0},//5
        {"Dash", 2, 11, 11, 0, 0},//6
        {"Pommel Strike", 1, 9, 0, 1, 0},//7
        {"Shrug It Off", 1, 0, 8, 1, 0},//8
        {"Battle Trance", 0, 0, 0, 2, 0},//9
        {"Seeing Red", 0, 0, 0, 0, 2}//10
        };
    */

    // Init Piles
    vector<int> drawPile = {0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> hand = { };
    vector<int> discardPile = { };

    // Init state machine
    GameState currentState = STATE_PLAYER_TURN;
    GameState lastState = STATE_PLAYER_TURN;

    // Init Turn
    int combatTurn = 1;
    vector<string> combatLogs = {"--- Combat start ---"};
    combatLogs.push_back("--- Player's Turn ---");

    // Init Damage
    vector<int> turnBasedDamage;
    int turnFinalCalculatedDamage = 0; 
    float totalDamageBonus = 0;

    // Init enemy intents
    int intent = 0; 
    
    // Init Card
    int playerSelectedCard = -1; 
    int doubleClickCardIndex = -1;

    // Init Questions
    QuestionGenerator g;
    QuizInfo currentQuiz;
    int totalQuestions = 0;
    int initialQuestions = 0;
    bool needNewQuestion = true;

    // Init Timer
    float quizTimer = 10.0f;
    float enemyTurnTimer = 1.0f; 

    // Init buttons
    GameButton endTurnBtn;
    GameButton skipBtn;
    GameButton viewDrawPileBtn;
    GameButton backToTurnBtn;     
    
    // Init button data
    endTurnBtn.init(1100, 410, 150, 50, "End Turn");
    skipBtn.init(320, 600, 150, 40, "Skip");
    viewDrawPileBtn.init(15, 560, 75, 85, "View", "Draw", "Pile");
    backToTurnBtn.init(15, 560, 75, 85, "Back", "To", "Turn");

    // Init Quiz buttons
    QuizButton quizButtons[4];
    quizButtons[0].init('A');
    quizButtons[1].init('B');
    quizButtons[2].init('C');
    quizButtons[3].init('D');

     // Init decks
    shuffleDeck(combatLogs, discardPile, drawPile); 
    drawCards(combatLogs, drawPile, hand, discardPile, 5);

    // Main Game Loop
    while (!WindowShouldClose()) {
        switch (currentState) {
            case STATE_PLAYER_TURN:{
                bool playCardTriggered = false;
                bool endTurnTriggered = false;
               
                if ((IsKeyPressed(KEY_ONE)   || IsKeyPressed(KEY_KP_1)) && hand.size() >= 1) playerSelectedCard = 0;
                if ((IsKeyPressed(KEY_TWO)   || IsKeyPressed(KEY_KP_2)) && hand.size() >= 2) playerSelectedCard = 1;
                if ((IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) && hand.size() >= 3) playerSelectedCard = 2;
                if ((IsKeyPressed(KEY_FOUR)  || IsKeyPressed(KEY_KP_4)) && hand.size() >= 4) playerSelectedCard = 3;
                if ((IsKeyPressed(KEY_FIVE)  || IsKeyPressed(KEY_KP_5)) && hand.size() >= 5) playerSelectedCard = 4;
                if ((IsKeyPressed(KEY_SIX)   || IsKeyPressed(KEY_KP_6)) && hand.size() >= 6) playerSelectedCard = 5;
                if ((IsKeyPressed(KEY_SEVEN) || IsKeyPressed(KEY_KP_7)) && hand.size() >= 7) playerSelectedCard = 6;
               if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) && playerSelectedCard != -1) {
                    playCardTriggered = true;
                }
                endTurnBtn.text1 = TextFormat("End Turn %d", combatTurn);
                int frameClickedIdx = drawDeck(hand, 530, Regular, p.getEnergy(), playerSelectedCard, true);
                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    playerSelectedCard = -1;
                }
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (frameClickedIdx != -1) {
                        if (playerSelectedCard == frameClickedIdx) {
                            playCardTriggered = true;
                        }
                        else {
                            playerSelectedCard = frameClickedIdx;
                        }
                    } 
                    else {
                        if (!endTurnBtn.isHovered && !viewDrawPileBtn.isHovered) {
                            playerSelectedCard = -1;
                        }
                    }
                }

                if (playCardTriggered && playerSelectedCard != -1) {
                    int cardID = hand[playerSelectedCard];
                    Card currentCard = CARD_DATABASE[cardID];
                    // Card logic
                    if (p.getEnergy() >= currentCard.cost) {
                        combatLogs.push_back(TextFormat("Played %s", currentCard.name.c_str()));
                        p.useEnergy(currentCard.cost);
                        if (currentCard.damage > 0){
                            totalQuestions++;
                            turnBasedDamage.push_back(currentCard.damage); 
                        }
                        if (currentCard.block > 0){
                            p.addBlock(currentCard.block, combatLogs);
                        }          
                        if(currentCard.drawNumber > 0){
                            drawCards(combatLogs, drawPile, hand, discardPile, currentCard.drawNumber);
                        }
                        if(currentCard.energyGained > 0){
                            p.gainEnergy(currentCard.energyGained, combatLogs);
                        }
                        // Move the card into discardPile              
                        discardPile.push_back(cardID);
                        hand.erase(hand.begin() + playerSelectedCard);
                        playerSelectedCard = -1; 
                    }
                    else {
                        combatLogs.push_back("Not enough energy!");
                        playerSelectedCard = -1;
                    }
                    playCardTriggered = false; 
                }

                if (endTurnBtn.isClicked(GetMousePosition())) {
                    endTurnTriggered = true;
                }
                if (viewDrawPileBtn.isClicked(GetMousePosition())) {
                    lastState = STATE_PLAYER_TURN;
                    currentState = STATE_VIEW_PILE;
                }
                if (IsKeyPressed(KEY_SPACE)) {
                    endTurnTriggered = true;
                }
                if (endTurnTriggered){
                    discardHand(hand, discardPile);
                    turnFinalCalculatedDamage = 0; 
                        if (totalQuestions > 0) {
                            initialQuestions = totalQuestions;
                            needNewQuestion = true;
                            currentState = STATE_MATH_QUIZ;
                            combatLogs.push_back("--- Math Quiz Start ---");
                        } 
                        else {
                            e.resetBlock();
                            currentState = STATE_ENEMY_TURN;
                            combatLogs.push_back("--- Enemy's Turn ---");
                        }
                }
                break;
            }
            case STATE_VIEW_PILE: {
                if (IsKeyPressed(KEY_SPACE)) {
                    currentState = lastState;
                }
                if (backToTurnBtn.isClicked(GetMousePosition())) {
                    currentState = lastState;
                }
                break;
            }
            case STATE_MATH_QUIZ:{
                if (needNewQuestion) {
                    currentQuiz = g.generateAlgebraQuestion();
                    quizTimer = 0.0f;
                    needNewQuestion = false;
                }
                Vector2 mousePos = GetMousePosition();
                // Question text starts here
                float questionX = 320.0f;
                float questionY = 280.0f; 

                // Atuomatic to new line
                string questionFullText = TextFormat("Question: %s", currentQuiz.title.c_str());
                Vector2 questionSize = MeasureTextEx(Regular, questionFullText.c_str(), 26, 1);

                // Dynammic change the question layout
                float startX = 320.0f;
                float startY = questionY + questionSize.y + 20.0f; 
                int fontSize = 22;
                float rowSpacing = 48.0f; 

                // Update quiz buttons 
                quizButtons[0].update(mousePos, currentQuiz.optA, startX, startY + (0 * rowSpacing), fontSize, Regular);
                quizButtons[1].update(mousePos, currentQuiz.optB, startX, startY + (1 * rowSpacing), fontSize, Regular);
                quizButtons[2].update(mousePos, currentQuiz.optC, startX, startY + (2 * rowSpacing), fontSize, Regular);
                quizButtons[3].update(mousePos, currentQuiz.optD, startX, startY + (3 * rowSpacing), fontSize, Regular);

                char playerChoice = '\0';
                if (IsKeyPressed(KEY_A)) playerChoice = 'A';
                if (IsKeyPressed(KEY_B)) playerChoice = 'B';
                if (IsKeyPressed(KEY_C)) playerChoice = 'C';
                if (IsKeyPressed(KEY_D)) playerChoice = 'D';

                // Skipped question 
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && quizTimer <= 10.0f) {
                    for (int i = 0; i < 4; i++) {
                        if (quizButtons[i].isHovered) {
                            playerChoice = quizButtons[i].choice;
                            break;
                        }
                    }
                } 
                bool skipTriggered = false;
                if (quizTimer <= 10.0f){
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && skipBtn.isHovered) || IsKeyPressed(KEY_SPACE)) {
                        skipTriggered = true;
                    }
                }
                
                // Times up 
                bool continueTriggered = false;
                if (quizTimer > 10.0f) {
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && skipBtn.isHovered) || IsKeyPressed(KEY_SPACE)) {
                        skipTriggered = true;
                    }   
                }
               if (playerChoice != '\0' || skipTriggered || continueTriggered) {
                    float damageBonus = 0.0f;
                    if (skipTriggered) {
                        combatLogs.push_back("Skipped question! Damage reduced to 0.");
                        damageBonus = 0.0f;
                    } 
                    else if (quizTimer > 10.0f) {
                        combatLogs.push_back("Too slow! Damage reduced 50%.");
                        damageBonus = 0.5f;
                    } 
                    else if (playerChoice == currentQuiz.correctAns) {
                        damageBonus = 1.0f + (10.0f / (20.0f + quizTimer * quizTimer));
                        combatLogs.push_back(TextFormat("Correct! Bonus: %.2fx", damageBonus));
                    } 
                    else {
                        combatLogs.push_back("Incorrect! Damage reduced 50%.");
                        damageBonus = 0.5f;
                    }
                    // Accumulating damage 
                    int currentCardId = initialQuestions - totalQuestions; 
                    float originalCardDamage = (float)turnBasedDamage[currentCardId];
                    turnFinalCalculatedDamage += (int)(originalCardDamage * damageBonus);
                    totalQuestions--;

                    if (totalQuestions > 0) {
                        needNewQuestion = true;
                    } 
                    else {
                        e.takeDamage(turnFinalCalculatedDamage, combatLogs);
                        turnBasedDamage.clear();
                        if (e.getHp() <= 0) {
                            currentState = STATE_GAME_OVER;
                        } 
                        else {
                            e.resetBlock(); 
                            currentState = STATE_ENEMY_TURN;
                            combatLogs.push_back("--- Enemy's Turn ---");
                        }
                    }
                }
                // Acculmulating timer
                if (quizTimer <= 10.0f &&  !needNewQuestion) {
                    quizTimer += GetFrameTime();
                }
                break;
            }
            case STATE_ENEMY_TURN: {
                enemyTurnTimer += GetFrameTime();
                if (enemyTurnTimer >= 1.0f) {
                    enemyTurnTimer = 0.0f; 
                    
                    e.changeIntent(p, combatLogs);
                    if (p.getHp() <= 0) {
                        currentState = STATE_GAME_OVER;
                        break;
                    }

                    currentState = STATE_PLAYER_TURN;
                    combatLogs.push_back("--- Player's Turn ---");
                    combatTurn++;
                    turnFinalCalculatedDamage = 0;
                    
                    p.resetEnergy();
                    p.resetBlock();
                    drawCards(combatLogs, drawPile, hand, discardPile, 5);
                }
                break;
            }
    }
        // Drawing
        BeginDrawing();
        ClearBackground(BLACK); 
        
        if (currentState != STATE_VIEW_PILE){
            string enemyIntent = "";
            string damgeText = ""; 

            int currentType = e.getType();
            int currentIndex = e.getIntentIndex();
            int currentAtk = e.getAtk();
            // Nibbit
            if (currentType == 0) { 
            if (currentIndex == 0) {
                enemyIntent = "Attack";
                damgeText = TextFormat("%d", currentAtk);
            } 
            else if (currentIndex == 1) {
                enemyIntent = "Attack + Defend";
                damgeText = TextFormat("%d", currentAtk);
            } 
            else {
                enemyIntent = "Buff";
                damgeText = "0";
            }
        }
        // Anthony
        else if (currentType == 1) { 
            if (currentIndex == 0) {
                enemyIntent = "Attack";
                damgeText = TextFormat("%d", currentAtk);
            } 
            else if (currentIndex == 1) {
                enemyIntent = "Attack + Defend";
                damgeText = TextFormat("%d", currentAtk);
            } 
            else {
                enemyIntent = "Stunned";
                damgeText = "0";
            }
        }
        // Bygone Effigy
        else if (currentType == 2) { 
            if (currentIndex == 0) {
                enemyIntent = "Sleep";
                damgeText = "0";
            } 
            else if (currentIndex == 1) {
                enemyIntent = "Buff";
                damgeText = "0";
            } 
            else {
                enemyIntent = "Attack";
                damgeText = TextFormat("%d", currentAtk);
            }
        }
        // Hexagonal Ghost
        else if (currentType == 3) { 
            if (currentIndex == 0) {
                enemyIntent = "Unknown";
                damgeText = "0";
            } 
            else if (currentIndex == 1) {
                enemyIntent = "Ultimate";
                int damage = (p.getHp() / 12) + 1;
                damgeText = TextFormat("%d x 6", damage);
            } 
            else if (currentIndex == 2) {
                enemyIntent = "Attack";
                damgeText = TextFormat("%d x 2", currentAtk);
            } 
            else if (currentIndex == 3) {
                enemyIntent = "Attack";
                damgeText = TextFormat("%d", currentAtk);
            } 
            else if (currentIndex == 4) {
                enemyIntent = "Defend";
                damgeText = "0";
            } 
            else if (currentIndex == 5) {
                enemyIntent = "Buff";
                damgeText = "0";
            } 
            else if (currentIndex == 6) {
                enemyIntent = "Unknown";
                damgeText = "0";
            } 
            else if (currentIndex == 7) {
                enemyIntent = "Ultimate";
                damgeText = TextFormat("%d x 6", currentAtk);
            } 
            else {
                enemyIntent = "Attack";
                damgeText = TextFormat("%d", currentAtk);
            }
        }

            int displayTotalDamage = 0;
            for (int i = 0; i < turnBasedDamage.size(); i++) {
                displayTotalDamage += turnBasedDamage[i];
            }

            // Display player and enemy info
            DrawTextEx(SemiBold, TextFormat("Player HP: %d / %d", p.getHp(), p.getMaxHp()), Vector2{50, 40}, 24, 1, BLUE);
            if (displayTotalDamage == 0) {
                DrawTextEx(SemiBold, TextFormat("Player Damage: ---"), Vector2{50, 70}, 24, 1, BLUE);
            } 
            else {
                DrawTextEx(SemiBold, TextFormat("Player Damage: %d", displayTotalDamage), Vector2{50, 70}, 24, 1, BLUE);
            }
            if (totalQuestions == 0) {
                DrawTextEx(SemiBold, TextFormat("Player Questions: ---"), Vector2{50, 100}, 24, 1, BLUE);
            } 
            else {
                DrawTextEx(SemiBold, TextFormat("Player Questions: %d", totalQuestions), Vector2{50, 100}, 24, 1, BLUE);
            }
            if (p.getBlock() == 0) {
                DrawTextEx(SemiBold, TextFormat("Player Block: --- "), Vector2{50, 130}, 24, 1, BLUE);
            } 
            else {
                DrawTextEx(SemiBold, TextFormat("Player Block: %d", p.getBlock()), Vector2{50, 130}, 24, 1, BLUE);
            }

            DrawTextEx(SemiBold, TextFormat("%s HP: %d / %d", e.getName().c_str(), e.getHp(), e.getMaxHp()), Vector2{900, 40}, 24, 1, RED);
            if (damgeText == "0") {
                DrawTextEx(SemiBold, TextFormat("%s ATK: ---", e.getName().c_str()), Vector2{900, 70}, 24, 1, RED);
            } 
            else {
                DrawTextEx(SemiBold, TextFormat("%s ATK: %s", e.getName().c_str(), damgeText.c_str()), Vector2{900, 70}, 24, 1, RED);
            }
            DrawTextEx(SemiBold, TextFormat("%s Intent: %s", e.getName().c_str(), enemyIntent.c_str()), Vector2{900, 100}, 24, 1, RED);
            if (e.getBlock() == 0) {
                DrawTextEx(SemiBold, TextFormat("%s Block: ---", e.getName().c_str()), Vector2{900, 130}, 24, 1, RED);
            } 
            else {
                DrawTextEx(SemiBold, TextFormat("%s Block: %d", e.getName().c_str(), e.getBlock()), Vector2{900, 130}, 24, 1, RED);
            }


            // Draw log 
            int logY = 10;
            float fixedLogX = 520.0f;   
            DrawTextEx(Bold, "Combat Logs:", Vector2 {520.0, 10}, 20, 1, GRAY);
            
            int startLog;
            if (combatLogs.size() > 6){
                startLog = combatLogs.size() - 6;
            }
            else{
                startLog = 0;
            }
            for (int i = startLog; i < combatLogs.size(); i++) {
                logY += 25;
                // Draw each log
                DrawTextEx(Bold, combatLogs[i].c_str(), Vector2{fixedLogX, (float)logY}, 20, 1, LIGHTGRAY);
            }
        }

        // Display different text based on game state
        if (currentState == STATE_PLAYER_TURN) {
            DrawTextEx(SemiBold, "Current State: [ PLAYER TURN ]", Vector2{320, 240}, 32, 1, GOLD);
            DrawTextEx(SemiBold, "Press [ SPACE ] or click the button to end your turn", Vector2{320, 280}, 26, 1, GRAY);

            float capsuleWidth = 68.0f;
            float capsuleHeight = 36.0f; 
            float energyX = 15.0f;      
            float energyY = 512.0f;     

            Rectangle energyRect = { energyX, energyY, capsuleWidth, capsuleHeight };
            Color energyBg = Color{ 210, 95, 30, 255 }; 
            DrawRectangleRounded(energyRect, 1.0f, 4, energyBg);
            DrawRectangleRoundedLinesEx(energyRect, 1.0f, 4, 2.0f, GOLD); 
            string energyText = TextFormat("%d/%d", p.getEnergy(), p.getMaxEnergy());
            float energyFontSize = 22.0f; 
            Vector2 textSize = MeasureTextEx(Bold, energyText.c_str(), energyFontSize, 1);
            Vector2 textPos = {
                energyX + (capsuleWidth - textSize.x) / 2.0f,
                energyY + (capsuleHeight - textSize.y) / 2.0f
            };
            DrawTextEx(Bold, energyText.c_str(), textPos, energyFontSize, 1, WHITE);
            drawDeck(hand, 530, Bold, p.getEnergy(), playerSelectedCard, false);
            viewDrawPileBtn.draw(SemiBold, Color{ 35, 45, 60, 255 }, Color{ 60, 80, 115, 255 }, Color{ 110, 125, 145, 255 });
            endTurnBtn.draw(Regular, Color{ 40, 40, 45, 255 }, Color{ 0, 185, 0, 255 }, Color{ 100, 100, 105, 255 });
        } 
        else if (currentState ==  STATE_VIEW_PILE) {
            backToTurnBtn.draw(SemiBold, Color{ 35, 45, 60, 255 }, Color{ 60, 80, 115, 255 }, Color{ 110, 125, 145, 255 });
        }
        else if (currentState == STATE_MATH_QUIZ) {
            DrawTextEx(Regular, "Current State: [ MATH QUIZ ]", Vector2{320,  240}, 32, 1, PURPLE);
                  
            // Print questions 
            string questionFullText = TextFormat("Question: %s", currentQuiz.title.c_str());
            DrawTextEx(Regular, questionFullText.c_str(), Vector2{320, 280}, 26, 1, WHITE);      
            Vector2 questionSize = MeasureTextEx(Regular, questionFullText.c_str(), 26, 1);
            float startX = 320.0f;
            float startY = 280.0f + questionSize.y + 20.0f; 
            float rowSpacing = 48.0f;

            for (int i = 0; i < 4; i++) {
                quizButtons[i].draw(22, Regular);
            }
            float timeBarY = startY + (4 * rowSpacing) + 5.0f; 
            float maxBarWidth = 400.0f;
            float currentBarWidth = maxBarWidth * (1.0f - (quizTimer / 10.0f));
            if (currentBarWidth < 0){
                currentBarWidth = 0; 
            }
            DrawRectangleLines(320, timeBarY, maxBarWidth, 15, GRAY);
            Color barColor = GREEN;
            float timeRatio = quizTimer / 10.0f;
            if (timeRatio > 1.0f){
                timeRatio = 1.0f;
            }
            if (quizTimer > 10.0f) {
                barColor = RED;
            } 
            else if (timeRatio < 0.5f) {
                float t = timeRatio / 0.5f; 
                int redValue = (int)(0 + t * 255);
                barColor.r = (char)redValue; 
                barColor.g = 255;
                barColor.b = 0;
            } 
            else {  
                float t = (timeRatio - 0.5f) / 0.5f;
                int greenValue = (int)(255 - t * 255);
                barColor.r = 255;
                barColor.g = (char)greenValue; 
                barColor.b = 0;
            }
            // Draw Time Bar
            DrawRectangle(320, timeBarY, currentBarWidth, 15, barColor);

            DrawTextEx(SemiBold, TextFormat("Total Damage Dealt: %d", turnFinalCalculatedDamage), Vector2{750, timeBarY - 3.0f}, 22, 1, RED);
            float skipBtnY = timeBarY + 25.0f;
            skipBtn.rect.y = skipBtnY; 
            if (quizTimer > 10.0f) {
                skipBtn.text1 = "Continue";
            } 
            else {
                skipBtn.text1 = "Skip";
            }
            skipBtn.draw(Regular, Color{ 60, 60, 60, 255 }, Color{ 185, 0, 0, 255 }, WHITE);
            if (quizTimer > 10.0f) {
                DrawTextEx(SemiBold, "TIME'S UP! Press Space or Click Continue.", Vector2{490, skipBtn.rect.y + 10}, 20  , 1, RED);
            } 
            else { 
                DrawTextEx(SemiBold, TextFormat("Time Left: %.1fs", 10.0f - quizTimer), Vector2{490, skipBtn.rect.y + 10}, 20, 1, ORANGE);
            }

            int backLogQuestions = totalQuestions;
            if (totalQuestions > 0){
                backLogQuestions = totalQuestions - 1;  
            }
            else{
                backLogQuestions = 0;
            }
            DrawTextEx(SemiBold, TextFormat("Quizzes Left: %d", backLogQuestions), Vector2{320, skipBtn.rect.y + 55.0f}, 22, 1, BEIGE);
        }
        else if (currentState == STATE_ENEMY_TURN) {
            DrawTextEx(Regular, "Current State: [ ENEMY TURN ]", Vector2{320, 240}, 32, 1, RED);
        }
        else if (currentState == STATE_GAME_OVER) {
            if (e.getHp() <= 0 ){
                DrawTextEx(Regular, "You win!", Vector2{320, 240}, 32, 1, RED);
            }
            if (p.getHp() <= 0 ){
                DrawTextEx(Regular, "You lose! Game Over", Vector2{320, 240}, 32, 1, RED);
            }
        }
        EndDrawing();
    }
    //Unload Resources
    UnloadFont(Thin);
    UnloadFont(Regular);
    UnloadFont(SemiBold);
    UnloadFont(Bold);
    CloseWindow();
}
