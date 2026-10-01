#include "raylib.h"
#include <iostream>
#include <vector>
#include <time.h> 
using namespace std;

enum GameState {
    STATE_MENU,
    STATE_PLAYER_TURN,
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
    Character(string charName, int initialHp, int charMaxHp, int initBlock) : name(charName), hp(initialHp), maxHp(charMaxHp), block(initBlock) {}
    string getName() const { return name; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getBlock() const { return block; }

    void addBlock(int blockValue, vector<string>& logs){
        logs.push_back(TextFormat("%s gained %d block", name.c_str(), blockValue));
        block += blockValue;
    }
    void takeDamage(int Damage, vector<string>& logs){
        if(block > Damage){
            logs.push_back(TextFormat("%s blocks %d damage", name.c_str(), Damage)); 
            block -= Damage;
        }
        else{
            Damage -= block;
            hp -= Damage;
            if (block > 0){
                logs.push_back(TextFormat("%s blocks %d damage", name.c_str(), block)); 
            }
            logs.push_back(TextFormat("%s takes %d damage", name.c_str(), Damage));
            block = 0;
        }
        if(hp <= 0){
            logs.push_back(TextFormat("%s is defeated", name.c_str()));
            hp = 0;
        }
    }
    void resetBlock(vector<string>& logs) {
        block = 0;
        logs.push_back(TextFormat("%s's block reset to 0", name.c_str()));
    }
};

class Player : public Character {
protected:
    int energy;
    int maxEnergy;

public:
    Player(string pName, int pHp, int pMaxHp, int pBlock, int pEnergy, int pMaxEnergy) 
        : Character(pName, pHp, pMaxHp, pBlock), energy(pEnergy), maxEnergy(pMaxEnergy) {}
        
    int getEnergy() const { return energy; }
    int getMaxEnergy() const { return maxEnergy; }

    void resetEnergy(vector<string>& logs) {
        energy = maxEnergy;
        logs.push_back(TextFormat("%s's energy reset to %d", name.c_str(), maxEnergy));
    }
    
    void useEnergy(int cardEnergyCost) {
        energy -= cardEnergyCost;
    }
};

class Enemy : public Character {
protected:
    int atk;

public:
    Enemy(string eName, int eHp, int eMaxHp, int eBlock, int eAtk) 
        : Character(eName, eHp, eMaxHp, eBlock), atk(eAtk) {}
        
    int getAtk() const { return atk; }
    int getBlock() const { return block;}

    void attack(Player& p, vector<string>& logs) {
        p.takeDamage(GetRandomValue(atk, atk), logs); 
    }
};

struct Card {
    string name;
    int cost;
    int damage;
    int block;
};

const Card CARD_DATABASE[7] = {
    {"Strike", 1, 6, 0},//0
    {"Defend", 1, 0, 5},//1
    {"Bludgeon", 3, 30, 0},//2
    {"IronWave", 1, 5, 5},//3
    {"Ultimate Strike", 1, 12, 0},//4
    {"Ultimate Defend", 1, 0, 11},//5
    {"Dash", 2, 10, 10},//6
};

void shuffleDeck(vector<string>& logs, vector<int>& discardPile, vector<int>& drawPile) {
    // Push the card into drawPile
    if (!discardPile.empty()){
        for (int i = 0; i < discardPile.size(); i++){
            drawPile.push_back(discardPile[i]);
        }
        discardPile.clear();
    }
    // Shuffle deck
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
        int textWidth = MeasureTextEx(font, "Your hand is empty!", 24, 1).x;
        DrawTextEx(font, "Your hand is empty!", Vector2{(1280.0f - textWidth) / 2.0f, (float)startY + 50}, 24, 1, DARKGRAY);
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
        Color borderColor = GRAY;
        if (isHovered || i == selectedCardIdx) {
            baseColor = Color{ 50, 50, 70, 255 };
            // Change border color
            if (playerEnergy >= currentCard.cost) {
                borderColor = GOLD;
            } 
            else {
                borderColor = RED; 
            }
        }

        // Draw Cards and border
        DrawRectangleRec(cardRect, baseColor);
        DrawRectangleLinesEx(cardRect, 1, borderColor);

        // Draw energy
        float energyCircleX = posX + 10;
        float energyCircleY = currentCardY; 
        DrawCircle(energyCircleX, energyCircleY, 18, ORANGE); 
        DrawCircleLines(energyCircleX, energyCircleY, 18, WHITE);
        DrawTextEx(font, TextFormat("%d", currentCard.cost), Vector2{energyCircleX - 8, energyCircleY - 14}, 30, 1, WHITE); 

        float textAlignX = posX + 15;
        string cardName = currentCard.name;
        int splitPos = (int)cardName.find_first_of("- "); 
        if (splitPos != -1) {
            string firstLine = cardName.substr(0, splitPos);
            string secondLine = cardName.substr(splitPos + 1);
            DrawTextEx(font, firstLine.c_str(), Vector2{textAlignX, currentCardY + 24}, 24, 1, WHITE);
            DrawTextEx(font, secondLine.c_str(), Vector2{textAlignX, currentCardY + 48}, 24, 1, WHITE);
        }
        else {
            DrawTextEx(font, cardName.c_str(), Vector2{textAlignX, currentCardY + 24}, 24, 1, WHITE);
        }

        // Draw ATK 
        if (currentCard.damage > 0) {
            DrawTextEx(font, TextFormat("ATK: %d", currentCard.damage), Vector2{textAlignX, currentCardY + 77}, 24, 1, RED);
        }

        // Draw DEF
        if (currentCard.block > 0) {
            float blockYOffset = (currentCard.damage > 0) ? (currentCardY + 101.0f) : (currentCardY + 77.0f);
            DrawTextEx(font, TextFormat("DEF: %d", currentCard.block), Vector2{textAlignX, blockYOffset}, 24, 1, BLUE);
        }
        
        // Draw Card Index 
        DrawTextEx(font, TextFormat("#%d", i + 1), Vector2{posX + cardWidth - 32, currentCardY + cardHeight - 22}, 16, 1, WHITE);
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

    // Draws the button box, border, and handles hover colors
    void draw(int fontSize, Font font) {
        Color boxColor = isHovered ? Color{ 60, 60, 60, 255 } : Color{ 30, 30, 30, 255 };
        Color borderColor = isHovered ? GOLD : GRAY;
        Color textColor = isHovered ? GOLD : WHITE;
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
        } else if (lengthType == 2) {
            // Simulated 2-line question using '\n'
            quiz.title = TextFormat("Advanced Algebra Problem (Turn %d):\nSimplify the basic linear expression: %d y - (-%d y) = ?", GetRandomValue(1, 5), A, B);
        } else {
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
        } else if (targetPos == 2) {
            quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, correctVal, wrong2, wrong3);
            quiz.optA = TextFormat("A. %dy", wrong1);
            quiz.optB = TextFormat("B. %dy", correctVal);
            quiz.optC = TextFormat("C. %dy", wrong2);
            quiz.optD = TextFormat("D. %dy", wrong3);
            quiz.correctAns = 'B';
        } else if (targetPos == 3) {
            quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, wrong2, correctVal, wrong3);
            quiz.optA = TextFormat("A. %dy", wrong1);
            quiz.optB = TextFormat("B. %dy", wrong2);
            quiz.optC = TextFormat("C. %dy", correctVal);
            quiz.optD = TextFormat("D. %dy", wrong3);
            quiz.correctAns = 'C';
        } else {
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
    Font customFont = LoadFont("resources/font/JetBrainsMono-Regular.ttf");

    // Init player
    Player p ("IronClad", 80, 80, 0, 3, 3);
    
    Enemy e ("Anthony", 150, 150, 0, 16);
    
    //InitDeck Info
    /*
const Card CARD_DATABASE[7] = {
    {"Strike", 1, 6, 0},//0
    {"Defend", 1, 0, 5},//1
    {"Bludgeon", 3, 30, 0},//2
    {"IronWave", 1, 5, 5},//3
    {"Ultimate Strike", 1, 12, 0},//4
    {"Ultimate Defend", 1, 0, 11},//5
    {"Dash", 2, 10, 10},//6
};
    */

    // Init Piles
    vector<int> drawPile = {0, 0, 0, 0, 1, 1, 1, 1, 2, 3, 4, 5, 6};
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
    int turnBaseDamage = 0;
    int turnFinalCalculatedDamage = 0; 
    float totalDamageBonus = 0;
    
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
    float quizTimer = 0.0f;
    float enemyTurnTimer = 0.0f; 

    // Init buttons
    Rectangle endTurnBtn = { 1100, 410, 150, 50 };
    Rectangle skipBtn = { 320, 600, 150, 40 };
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
               
                //Keyboard numbers to play card
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

                // Get click index
                int frameClickedIdx = drawDeck(hand, 530, customFont, p.getEnergy(), playerSelectedCard, true);
                
                // Mouse input
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
                        if (!CheckCollisionPointRec(GetMousePosition(), endTurnBtn)) {
                            playerSelectedCard = -1;
                        }
                    }
                }

                if (playCardTriggered && playerSelectedCard != -1) {
                    int cardID = hand[playerSelectedCard];
                    Card currentCard = CARD_DATABASE[cardID];
                    if (p.getEnergy() >= currentCard.cost) {
                        combatLogs.push_back(TextFormat("Played %s", currentCard.name.c_str()));
                        p.useEnergy(currentCard.cost);
                        if (currentCard.damage > 0){
                            totalQuestions++;
                            turnBaseDamage += currentCard.damage; 
                            combatLogs.push_back(TextFormat("turnBaseDamage + %d", currentCard.damage));
                        }
                        if (currentCard.block > 0){
                            p.addBlock(currentCard.block, combatLogs);
                        }                        
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

                // Click end turn button
                if (CheckCollisionPointRec(GetMousePosition(), endTurnBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    endTurnTriggered = true;
                }

                // Space
                if (IsKeyPressed(KEY_SPACE)) {
                    endTurnTriggered = true;
                }

                if (endTurnTriggered){
                    discardHand(hand, discardPile);
                    combatLogs.push_back(TextFormat("%s ended turn %d", p.getName().c_str(), combatTurn));
                    turnFinalCalculatedDamage = 0; 
                        if (totalQuestions > 0) {
                            // Do damage
                            initialQuestions = totalQuestions;
                            needNewQuestion = true;
                            currentState = STATE_MATH_QUIZ;
                            combatLogs.push_back("--- Math Quiz Start ---");
                        } else {
                            // No damage
                            currentState = STATE_ENEMY_TURN;
                            combatLogs.push_back("--- Enemy's Turn ---");
                        }
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
                Vector2 questionSize = MeasureTextEx(customFont, questionFullText.c_str(), 26, 1);

                // Dynammic change the question layout
                float startX = 320.0f;
                float startY = questionY + questionSize.y + 20.0f; 

                int fontSize = 22;
                float rowSpacing = 48.0f; 

                // Update quiz buttons 
                quizButtons[0].update(mousePos, currentQuiz.optA, startX, startY + (0 * rowSpacing), fontSize, customFont);
                quizButtons[1].update(mousePos, currentQuiz.optB, startX, startY + (1 * rowSpacing), fontSize, customFont);
                quizButtons[2].update(mousePos, currentQuiz.optC, startX, startY + (2 * rowSpacing), fontSize, customFont);
                quizButtons[3].update(mousePos, currentQuiz.optD, startX, startY + (3 * rowSpacing), fontSize, customFont);

                // Acculmulating timer
                 if (quizTimer <= 6.0f) {
                    quizTimer += GetFrameTime();
                }

                char playerChoice = '\0';
                if (IsKeyPressed(KEY_A)) playerChoice = 'A';
                if (IsKeyPressed(KEY_B)) playerChoice = 'B';
                if (IsKeyPressed(KEY_C)) playerChoice = 'C';
                if (IsKeyPressed(KEY_D)) playerChoice = 'D';

                // Skip question by players
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && quizTimer <= 6.0f) {
                    for (int i = 0; i < 4; i++) {
                        if (quizButtons[i].isHovered) {
                            playerChoice = quizButtons[i].choice;
                            break;
                        }
                    }
                } 
                bool skipTriggered = false;
                if (quizTimer <= 6.0f){
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), skipBtn) || IsKeyPressed(KEY_SPACE)) {
                        skipTriggered = true;
                    }
                }
                
                // Times up logic
                bool continueTriggered = false;
                if (quizTimer > 6.0f) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), skipBtn) || IsKeyPressed(KEY_SPACE)) {
                        continueTriggered = true;
                    }
                }

               if (playerChoice != '\0' || skipTriggered || continueTriggered) {
                    float damageBonus = 0.0f;
                    if (skipTriggered) {
                        combatLogs.push_back("Skipped question! Damage reduced to 0.");
                        damageBonus = 0.0f;
                    } else if (quizTimer > 6.0f) {
                        combatLogs.push_back("Too slow! Damage reduced 50%.");
                        damageBonus = 0.5f;
                    } else if (playerChoice == currentQuiz.correctAns) {
                        damageBonus = 1.0f + (6.0f / (20.0f + quizTimer * quizTimer));
                        combatLogs.push_back(TextFormat("Correct! Bonus: %.2fx", damageBonus));
                    } else {
                        combatLogs.push_back("Incorrect! Damage reduced 50%.");
                        damageBonus = 0.5f;
                    }

                    float singleCardBaseDamage = (float)turnBaseDamage / initialQuestions;
                    turnFinalCalculatedDamage += (int)(singleCardBaseDamage * damageBonus);
                    totalQuestions--;
                    if (totalQuestions > 0) {
                        needNewQuestion = true;
                    } 
                    else {
                        e.takeDamage(turnFinalCalculatedDamage, combatLogs);
                        turnBaseDamage = 0;
                        turnFinalCalculatedDamage = 0;

                        if (e.getHp() <= 0) {
                            currentState = STATE_GAME_OVER;
                        } else {
                            currentState = STATE_ENEMY_TURN;
                            combatLogs.push_back("--- Enemy's Turn ---");
                        }
                    }
                }
                break;
            }
            case STATE_ENEMY_TURN:{
                if (enemyTurnTimer == 0.0f) {
                    e.attack(p, combatLogs);
                    // End game when player is defeated
                    if (p.getHp() <= 0) {
                        currentState = STATE_GAME_OVER;
                        enemyTurnTimer = 0.0f;
                        break;
                    }
                }
                enemyTurnTimer += GetFrameTime();
                if (enemyTurnTimer >= 1.0f) {
                    enemyTurnTimer = 0.0f; 
                    // Player's turn
                    currentState = STATE_PLAYER_TURN;
                    combatLogs.push_back("--- Player's Turn ---");
                    combatTurn++;
                    p.resetEnergy(combatLogs);
                    p.resetBlock(combatLogs);
                    drawCards(combatLogs, drawPile, hand, discardPile, 5);
                    //Enemy gains block (random)
                    e.resetBlock(combatLogs);
                    e.addBlock(GetRandomValue(0, 5), combatLogs);
                }
                break;
            }
    }
        // Drawing
        BeginDrawing();
        ClearBackground(BLACK); 
        
        //Enemy's intent classification
        string enemyIntent = "";
        if (e.getBlock() > 0 && e.getAtk() > 0) {
            enemyIntent = "Strategic Attack";
        } else if (e.getAtk() > 0) {
            enemyIntent = "Attack";
        } else if (e.getBlock() > 0) {
            enemyIntent = "Strategic";
        }

        // Display player info
        DrawTextEx(customFont, TextFormat("Player HP: %d / %d", p.getHp(), p.getMaxHp()), Vector2{50, 40}, 24, 1, BLUE);
        DrawTextEx(customFont, TextFormat("Player Damage: %d", turnBaseDamage), Vector2{50, 70}, 24, 1, BLUE);
        DrawTextEx(customFont, TextFormat("Player Questions: %d", totalQuestions), Vector2{50, 100}, 24, 1, BLUE);
        DrawTextEx(customFont, TextFormat("Player Block: %d", p.getBlock()), Vector2{50, 130}, 24, 1, BLUE);

        // Display enemy info
        DrawTextEx(customFont, TextFormat("%s HP: %d / %d", e.getName().c_str(), e.getHp(), e.getMaxHp()), Vector2{900, 40}, 24, 1, RED);
        DrawTextEx(customFont, TextFormat("%s ATK: %d", e.getName().c_str(), e.getAtk()), Vector2{900, 70}, 24, 1, RED);
        DrawTextEx(customFont, TextFormat("%s Intent: %s", e.getName().c_str(), enemyIntent.c_str()), Vector2{900, 100}, 24, 1, RED);
        DrawTextEx(customFont, TextFormat("%s BLOCK: %d", e.getName().c_str(), e.getBlock()), Vector2{900, 130}, 24, 1, RED);

        // Display different text based on game state
        if (currentState == STATE_PLAYER_TURN) {
            DrawTextEx(customFont, "Current State: [ PLAYER TURN ]", Vector2{320, 240}, 32, 1, GOLD);
            DrawTextEx(customFont, "Press [ SPACE ] or click the button to end your turn", Vector2{320, 280}, 26, 1, GRAY);

            float capsuleWidth = 68.0f;
            float capsuleHeight = 36.0f; 
            float energyX = 15.0f;      
            float energyY = 512.0f;     

            Rectangle energyRect = { energyX, energyY, capsuleWidth, capsuleHeight };

            Color energyBg = Color{ 210, 95, 30, 255 }; 
            DrawRectangleRounded(energyRect, 1.0f, 4, energyBg);
            DrawRectangleRoundedLines(energyRect, 1.0f, 4, GOLD); 

            string energyText = TextFormat("%d/%d", p.getEnergy(), p.getMaxEnergy());
            float energyFontSize = 22.0f; 
            Vector2 textSize = MeasureTextEx(customFont, energyText.c_str(), energyFontSize, 1);
            
            // Center text inside the narrow capsule
            Vector2 textPos = {
                energyX + (capsuleWidth - textSize.x) / 2.0f,
                energyY + (capsuleHeight - textSize.y) / 2.0f
            };
            DrawTextEx(customFont, energyText.c_str(), textPos, energyFontSize, 1, WHITE);

            // Draw deck
            drawDeck(hand, 530, customFont, p.getEnergy(), playerSelectedCard, false);

            // End Turn Button 
            if (!CheckCollisionPointRec(GetMousePosition(), endTurnBtn)){
                DrawRectangleRec(endTurnBtn, Color{ 200, 200, 200, 255 });
            }
            else{
                DrawRectangleRec(endTurnBtn, Color{ 0, 185, 0, 255 });
            }
            DrawTextEx(customFont, TextFormat("End Turn %d", combatTurn), Vector2{endTurnBtn.x + 18, endTurnBtn.y + 15}, 20, 1, BLACK); 
        } 
        else if (currentState == STATE_MATH_QUIZ) {
            DrawTextEx(customFont, "Current State: [ MATH QUIZ ]", Vector2{320,  240}, 32, 1, PURPLE);
                  
            // Print questions 
            string questionFullText = TextFormat("Question: %s", currentQuiz.title.c_str());
            DrawTextEx(customFont, questionFullText.c_str(), Vector2{320, 280}, 26, 1, WHITE);      

            // Output calculations according to its length
            Vector2 questionSize = MeasureTextEx(customFont, questionFullText.c_str(), 26, 1);
            float startX = 320.0f;
            float startY = 280.0f + questionSize.y + 20.0f; 
            float rowSpacing = 48.0f;

            for (int i = 0; i < 4; i++) {
                // Draw Quiz Buttons vertically aligned
                quizButtons[i].draw(22, customFont);
            }

            float timeBarY = startY + (4 * rowSpacing) + 5.0f; 
            float maxBarWidth = 400.0f;
            float currentBarWidth = maxBarWidth * (1.0f - (quizTimer / 6.0f));
            if (currentBarWidth < 0) currentBarWidth = 0; 
            DrawRectangleLines(320, timeBarY, maxBarWidth, 15, GRAY);

            // Color gradient calculations
            Color barColor = GREEN;
            float timeRatio = quizTimer / 6.0f;

            if (timeRatio > 1.0f) timeRatio = 1.0f;
            if (quizTimer > 6.0f) {
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

            float skipBtnY = timeBarY + 30.0f;
            skipBtn.y = skipBtnY; 

            // Draw skipButton
            if (!CheckCollisionPointRec(GetMousePosition(), skipBtn)){
                DrawRectangleRec(skipBtn, Color{ 60, 60, 60, 255 });
            }
            else{
                DrawRectangleRec(skipBtn, Color{ 185, 0, 0, 255 });
            }
            
            // Times up logic and drawing 
            if (quizTimer > 6.0f) {
                DrawTextEx(customFont, "Continue", Vector2{skipBtn.x + 30, skipBtn.y + 10}, 20, 1, YELLOW);
                DrawTextEx(customFont, "TIME'S UP! Press Space or Click Continue.", Vector2{490, skipBtn.y + 10}, 18, 1, RED);
            } 
            else { 
                DrawTextEx(customFont, "Skip", Vector2{skipBtn.x + 55, skipBtn.y + 10}, 20, 1, WHITE);
                DrawTextEx(customFont, TextFormat("Time Left: %.1fs", 6.0f - quizTimer), Vector2{490, skipBtn.y + 10}, 20, 1, ORANGE);
            }
            // Draw numbers of left quizzes
            int backLogQuestions = totalQuestions;
            if (totalQuestions > 0){
                backLogQuestions = totalQuestions - 1;
            }
            else{
                backLogQuestions = 0;
            }
            DrawTextEx(customFont, TextFormat("Quizzes Left: %d", backLogQuestions), Vector2{320, skipBtn.y + 55.0f}, 20, 1, BEIGE);
        }
        else if (currentState == STATE_ENEMY_TURN) {
            DrawTextEx(customFont, "Current State: [ ENEMY TURN ]", Vector2{320, 240}, 32, 1, RED);
        }
        else if (currentState == STATE_GAME_OVER) {
            if (e.getHp() <= 0 ){
                DrawTextEx(customFont, "You win!", Vector2{320, 240}, 32, 1, RED);
            }
            if (p.getHp() <= 0 ){
                DrawTextEx(customFont, "You lose! Game Over", Vector2{320, 240}, 32, 1, RED);
            }
        }

        // Draw log 
        int logY = 10;
        float fixedLogX = 520.0f;   
        DrawTextEx(customFont, "Combat Logs:", Vector2 {520.0, 10}, 20, 1, GRAY);
   
        int startLog = (combatLogs.size() > 6) ? combatLogs.size() - 6 : 0;
        for (int i = startLog; i < combatLogs.size(); i++) {
            logY += 25;
            // Draw each log
            DrawTextEx(customFont, combatLogs[i].c_str(), Vector2{fixedLogX, (float)logY}, 18, 1, LIGHTGRAY);
        }
        EndDrawing();
    }
    //Unload Resources
    UnloadFont(customFont);
    CloseWindow();
}
