#include "raylib.h"
#include "Card.h"
#include "Character.h"
#include "QuestionGenerator.h"
#include "UIComponents.h"
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

vector<CardMoveAnimation> cardAnimations;
bool isCardAnimationPlaying = false;
bool waitingForDiscardAnimation = false;
GameState nextStateAfterDiscard;
vector<ShuffleCardAnimation> shuffleAnimations;
bool isShuffleAnimationPlaying = false;
bool waitingForShuffleAnimation = false;
int pendingDrawNumber = 0;

void shuffleDeck(vector<string>& logs, vector<int>& discardPile, vector<int>& drawPile, vector<ShuffleCardAnimation>& shuffleAnimations) {
    if (!discardPile.empty()) {
        const float cardWidth = 140.0f;
        const float cardHeight = 180.0f;
        for (int i = 0; i < (int)discardPile.size(); i++) {
            ShuffleCardAnimation animation;
            animation.cardID = discardPile[i];
            animation.startPosition = { 1215.0f - cardWidth / 2.0f, 602.5f - cardHeight / 2.0f };
            animation.targetPosition = { 52.5f - cardWidth / 2.0f, 602.5f - cardHeight / 2.0f };
            animation.progress = 0.0f;
            animation.delay = GetRandomValue(0, 8) / 100.0f;
            animation.speed = GetRandomValue(125, 170) / 100.0f;
            animation.arcHeight = (float)GetRandomValue(130, 190);
            animation.isActive = false;
            shuffleAnimations.push_back(animation);
            drawPile.push_back(discardPile[i]);
        }
        discardPile.clear();
    }
    if (!drawPile.empty()) {
        for (int i = 0; i < (int)drawPile.size(); i++) {
            int swapIndex = GetRandomValue(0, (int)drawPile.size() - 1);

            int tempCard = drawPile[i];
            drawPile[i] = drawPile[swapIndex];
            drawPile[swapIndex] = tempCard;
        }
        logs.push_back("Cards reshuffled");
    }
}

void drawCards(vector<string>& logs, vector<int>& drawPile, vector<int>& hand, vector<int>& discardPile, vector<CardMoveAnimation>& cardAnimations, int numToDraw) {
    if (!shuffleAnimations.empty()) {
        pendingDrawNumber += numToDraw;
        waitingForShuffleAnimation = true;
        return;
    }
    while (numToDraw > 0) {
        if (drawPile.empty()) {
            shuffleDeck(logs, discardPile, drawPile, shuffleAnimations);
            if (!shuffleAnimations.empty()) {
                pendingDrawNumber += numToDraw;
                waitingForShuffleAnimation = true;
                return;
            }
            if (drawPile.empty()) {
                logs.push_back("Not enough cards to draw");
                break;
            }
        }   

        int topCardID = drawPile.back();
        if (hand.size() + cardAnimations.size() < 7) {
            CardMoveAnimation animation;
            animation.cardID = topCardID;
            animation.startPosition = { 0.0f, 0.0f };
            animation.targetPosition = { 0.0f, 0.0f };
            animation.progress = 0.0f;
            animation.delay = 0.0f;
            animation.isActive = false;
            animation.isDiscarding = false;
            cardAnimations.push_back(animation);
        }
        else {
            discardPile.push_back(topCardID);
            logs.push_back("Hand is full");
        }
        drawPile.pop_back();
        numToDraw--;
    }
}

void discardHand(vector<int>& hand, vector<CardMoveAnimation>& cardAnimations) {
    if (!hand.empty()) {
        const float cardWidth = 140.0f;
        const float cardHeight = 180.0f;
        const float cardSpacing = 16.0f;
        int numCards = (int)hand.size();
        float totalWidth = numCards * cardWidth + (numCards - 1) * cardSpacing;
        float startX = (1280.0f - totalWidth) / 2.0f;

        for (int i = 0; i < numCards; i++) {
            CardMoveAnimation animation;
            animation.cardID = hand[i];
            animation.startPosition = { startX + i * (cardWidth + cardSpacing), 490.0f };
            animation.targetPosition = { 1215.0f - cardWidth / 2.0f, 602.5f - cardHeight / 2.0f };
            animation.progress = 0.0f;
            animation.delay = 0.0f;
            animation.isActive = false;
            animation.isDiscarding = true;
            cardAnimations.push_back(animation);
        }
        hand.clear();
    }
}

int main() {
    int screenWidth = 1280;
    int screenHeight = 720;
    const int virtualScreenWidth = 1280;
    const int virtualScreenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Slay the Math");
    SetWindowMinSize(1280, 720);
    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    SetTargetFPS(60);

    Image icon = LoadImage("assets/img/icon.png");
    ImageResize(&icon, 64, 64);
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);

    SetRandomSeed((unsigned int)time(NULL));

    int difficulty = 3;

    Font Thin = LoadFont("assets/fonts/JetBrainsMono-Thin.ttf");
    Font Regular = LoadFont("assets/fonts/JetBrainsMono-Regular.ttf");
    Font SemiBold = LoadFont("assets/fonts/JetBrainsMono-SemiBold.ttf");
    Font Bold = LoadFont("assets/fonts/JetBrainsMono-Bold.ttf");

    Player p("IronClad", 80, 80, 0, 3, 3);
    Enemy e("Dummy", 114514, 114514, 202266, 234, 20, 0);

    int enemyType = GetRandomValue(0, 3);

    if (enemyType == 0) {
        e = Enemy("Nibbit", 96, 96, 0, 15, 3, 0);
    }
    else if (enemyType == 1) {
        e = Enemy("Anthony", 112, 112, 16, 16, 1, 1);
    }
    else if (enemyType == 2) {
        e = Enemy("Bygone Effigy", 156, 156, 0, 10, 10, 2);
    }
    else {
        e = Enemy("Hexagonal Ghost", 304, 304, 0, 4, 1, 3);
    }

    vector<int> drawPile = { 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 };
    vector<int> hand = {};
    vector<int> discardPile = {};

    GameState currentState = STATE_PLAYER_TURN;
    GameState lastState = STATE_PLAYER_TURN;

    int combatTurn = 1;
    vector<string> combatLogs = { "--- Combat start ---" };
    combatLogs.push_back("--- Player's Turn ---");

    vector<int> turnBasedDamage;
    int turnFinalCalculatedDamage = 0;

    int playerSelectedCard = -1;

    QuestionGenerator g;
    QuizInfo currentQuiz;
    int totalQuestions = 0;
    int initialQuestions = 0;
    bool needNewQuestion = true;

    float quizTimer = 10.0f;
    float enemyTurnTimer = 0.0f;

    gameButton endTurnBtn;
    gameButton skipBtn;
    gameButton viewDrawPileBtn;
    gameButton viewDiscardPileBtn;
    gameButton backToTurnBtn;

    bool viewingDiscardPile = false;

    endTurnBtn.init(1100, 410, 150, 50, "End Turn");
    skipBtn.init(320, 600, 150, 40, "Skip");
    viewDrawPileBtn.init(15, 560, 75, 85, "View", "Draw", "Pile");
    viewDiscardPileBtn.init(1165, 560, 100, 85, "View", "Discard", "Pile");
    backToTurnBtn.init(15, 560, 75, 85, "Back", "To", "Turn");

    quizButton quizButtons[4];
    quizButtons[0].init('A');
    quizButtons[1].init('B');
    quizButtons[2].init('C');
    quizButtons[3].init('D');

    shuffleDeck(combatLogs, discardPile, drawPile, shuffleAnimations);
    drawCards(combatLogs, drawPile, hand, discardPile, cardAnimations, 5);

    while (!WindowShouldClose()) {
        int currentW = GetScreenWidth();
        int currentH = GetScreenHeight();
        int expectedH = (int)((float)currentW * 9.0f / 16.0f);

        if (currentH != expectedH) {
            SetWindowSize(currentW, expectedH);
            currentW = GetScreenWidth();
            currentH = GetScreenHeight();
        }

        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();
        float scale = min((float)currentW / 1280.0f, (float)currentH / 720.0f);

        float offsetX = (currentW - (1280.0f * scale)) * 0.5f;
        float offsetY = (currentH - (720.0f * scale)) * 0.5f;

        Vector2 realMousePos = GetMousePosition();
        Vector2 virtualMousePos = { 0 };

        virtualMousePos.x = (realMousePos.x - offsetX) / scale;
        virtualMousePos.y = (realMousePos.y - offsetY) / scale;

        if (virtualMousePos.x < 0) virtualMousePos.x = 0;
        if (virtualMousePos.x > 1280) virtualMousePos.x = 1280;
        if (virtualMousePos.y < 0) virtualMousePos.y = 0;
        if (virtualMousePos.y > 720) virtualMousePos.y = 720;

        if (!cardAnimations.empty()) {
            CardMoveAnimation& animation = cardAnimations[0];

            if (animation.delay > 0.0f) {
                animation.delay -= GetFrameTime();
            }
            else {
                if (!animation.isActive) {
                    if (!animation.isDiscarding) {
                        animation.startPosition = { 52.5f - 70.0f, 602.5f - 90.0f };

                        int newHandSize = (int)hand.size() + 1;

                        float cardWidth = 140.0f;
                        float cardSpacing = 16.0f;

                        float totalWidth = newHandSize * cardWidth + (newHandSize - 1) * cardSpacing;
                        float startX = (1280.0f - totalWidth) / 2.0f;

                        animation.targetPosition = { startX + hand.size() * (cardWidth + cardSpacing), 490.0f };
                    }

                    animation.progress = 0.0f;
                    animation.isActive = true;
                }

                animation.progress += GetFrameTime() * 7.5f;

                if (animation.progress >= 1.0f) {
                    animation.progress = 1.0f;

                    if (animation.isDiscarding) {
                        discardPile.push_back(animation.cardID);
                    }
                    else {
                        hand.push_back(animation.cardID);
                    }

                    cardAnimations.erase(cardAnimations.begin());

                    if (!cardAnimations.empty()) {
                        cardAnimations[0].delay = 0.001f;
                    }
                    else if (waitingForDiscardAnimation) {
                        waitingForDiscardAnimation = false;

                        currentState = nextStateAfterDiscard;

                        if (currentState == STATE_MATH_QUIZ) {
                            combatLogs.push_back("--- Math Quiz Start ---");
                        }
                        else {
                            e.resetBlock();
                            combatLogs.push_back("--- Enemy's Turn ---");
                        }
                    }
                }
            }
        }

        for (int i = (int)shuffleAnimations.size() - 1; i >= 0; i--) {
            ShuffleCardAnimation& animation = shuffleAnimations[i];

            if (animation.delay > 0.0f) {
                animation.delay -= GetFrameTime();
                continue;
            }

            animation.isActive = true;
            animation.progress += GetFrameTime() * animation.speed;

            if (animation.progress >= 1.0f) {
                animation.progress = 1.0f;
                shuffleAnimations.erase(shuffleAnimations.begin() + i);
            }
        }

        isShuffleAnimationPlaying = !shuffleAnimations.empty();

        if (shuffleAnimations.empty() && waitingForShuffleAnimation) {
            int cardsToDraw = pendingDrawNumber;

            pendingDrawNumber = 0;
            waitingForShuffleAnimation = false;

            drawCards(combatLogs, drawPile, hand, discardPile, cardAnimations, cardsToDraw);
        }

        switch (currentState) {
            case STATE_PLAYER_TURN: {
                if (isCardAnimationPlaying || isShuffleAnimationPlaying) {
                    break;
                }

                bool playCardTriggered = false;
                bool endTurnTriggered = false;

                if ((IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) && hand.size() >= 1) playerSelectedCard = 0;
                if ((IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) && hand.size() >= 2) playerSelectedCard = 1;
                if ((IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) && hand.size() >= 3) playerSelectedCard = 2;
                if ((IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) && hand.size() >= 4) playerSelectedCard = 3;
                if ((IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) && hand.size() >= 5) playerSelectedCard = 4;
                if ((IsKeyPressed(KEY_SIX) || IsKeyPressed(KEY_KP_6)) && hand.size() >= 6) playerSelectedCard = 5;
                if ((IsKeyPressed(KEY_SEVEN) || IsKeyPressed(KEY_KP_7)) && hand.size() >= 7) playerSelectedCard = 6;

                if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) && playerSelectedCard != -1) {
                    playCardTriggered = true;
                }

                endTurnBtn.text1 = TextFormat("End Turn %d", combatTurn);

                int frameClickedIdx = drawDeck(hand, 490, Regular, p.getEnergy(), playerSelectedCard, true, virtualMousePos);

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
                        if (!endTurnBtn.isHovered && !viewDrawPileBtn.isHovered && !viewDiscardPileBtn.isHovered) {
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

                        if (currentCard.damage > 0) {
                            totalQuestions++;
                            turnBasedDamage.push_back(currentCard.damage);
                        }

                        if (currentCard.block > 0) {
                            p.addBlock(currentCard.block, combatLogs);
                        }

                        if (currentCard.drawNumber > 0) {
                            drawCards(combatLogs, drawPile, hand, discardPile, cardAnimations, currentCard.drawNumber);
                        }

                        if (currentCard.energyGained > 0) {
                            p.gainEnergy(currentCard.energyGained, combatLogs);
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

                if (viewDrawPileBtn.isClicked(virtualMousePos)) {
                    viewingDiscardPile = false;
                    lastState = currentState;
                    currentState = STATE_VIEW_PILE;
                }

                if (viewDiscardPileBtn.isClicked(virtualMousePos)) {
                    viewingDiscardPile = true;
                    lastState = currentState;
                    currentState = STATE_VIEW_PILE;
                }

                if (endTurnBtn.isClicked(virtualMousePos)) {
                    endTurnTriggered = true;
                }

                if (IsKeyPressed(KEY_SPACE)) {
                    endTurnTriggered = true;
                }

                if (endTurnTriggered) {
                    discardHand(hand, cardAnimations);

                    turnFinalCalculatedDamage = 0;

                    if (totalQuestions > 0) {
                        initialQuestions = totalQuestions;
                        needNewQuestion = true;
                        nextStateAfterDiscard = STATE_MATH_QUIZ;
                    }
                    else {
                        nextStateAfterDiscard = STATE_ENEMY_TURN;
                    }

                    if (!cardAnimations.empty()) {
                        waitingForDiscardAnimation = true;
                    }
                    else {
                        currentState = nextStateAfterDiscard;

                        if (currentState == STATE_MATH_QUIZ) {
                            combatLogs.push_back("--- Math Quiz Start ---");
                        }
                        else {
                            e.resetBlock();
                            combatLogs.push_back("--- Enemy's Turn ---");
                        }
                    }
                }

                break;
            }

            case STATE_VIEW_PILE: {
                if (IsKeyPressed(KEY_SPACE)) {
                    currentState = lastState;
                }

                if (backToTurnBtn.isClicked(virtualMousePos)) {
                    currentState = lastState;
                }

                break;
            }

            case STATE_MATH_QUIZ: {
                if (isCardAnimationPlaying) {
                    break;
                }

                if (needNewQuestion) {
                    currentQuiz = g.generateAlgebraQuestion(difficulty);
                    quizTimer = 0.0f;
                    needNewQuestion = false;
                }

                float questionX = 320.0f;
                float questionY = 280.0f;

                string questionFullText = TextFormat("Question: %s", currentQuiz.title.c_str());
                Vector2 questionSize = MeasureTextEx(Regular, questionFullText.c_str(), 26, 1);

                float startX = 320.0f;
                float startY = questionY + questionSize.y + 20.0f;
                int fontSize = 22;
                float rowSpacing = 48.0f;

                quizButtons[0].update(virtualMousePos, currentQuiz.optA, startX, startY + (0 * rowSpacing), fontSize, Regular);
                quizButtons[1].update(virtualMousePos, currentQuiz.optB, startX, startY + (1 * rowSpacing), fontSize, Regular);
                quizButtons[2].update(virtualMousePos, currentQuiz.optC, startX, startY + (2 * rowSpacing), fontSize, Regular);
                quizButtons[3].update(virtualMousePos, currentQuiz.optD, startX, startY + (3 * rowSpacing), fontSize, Regular);

                char playerChoice = '\0';

                if (IsKeyPressed(KEY_A)) playerChoice = 'A';
                if (IsKeyPressed(KEY_B)) playerChoice = 'B';
                if (IsKeyPressed(KEY_C)) playerChoice = 'C';
                if (IsKeyPressed(KEY_D)) playerChoice = 'D';

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && quizTimer <= 10.0f) {
                    for (int i = 0; i < 4; i++) {
                        if (quizButtons[i].isHovered) {
                            playerChoice = quizButtons[i].choice;
                            break;
                        }
                    }
                }

                bool skipTriggered = false;

                if (quizTimer <= 10.0f) {
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && skipBtn.isHovered) || IsKeyPressed(KEY_SPACE)) {
                        skipTriggered = true;
                    }
                }

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

                if (quizTimer <= 10.0f && !needNewQuestion) {
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

                    drawCards(combatLogs, drawPile, hand, discardPile, cardAnimations, 5);
                }

                break;
            }
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (currentState != STATE_VIEW_PILE && currentState != STATE_MENU) {
            string enemyIntent = "";
            string damgeText = "";

            int currentType = e.getType();
            int currentIndex = e.getIntentIndex();
            int currentAtk = e.getAtk();

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

            DrawTextEx(SemiBold, TextFormat("Player HP: %d / %d", p.getHp(), p.getMaxHp()), Vector2{ 50, 40 }, 24, 1, BLUE);

            if (displayTotalDamage == 0) {
                DrawTextEx(SemiBold, TextFormat("Player Damage: ---"), Vector2{ 50, 70 }, 24, 1, BLUE);
            }
            else {
                DrawTextEx(SemiBold, TextFormat("Player Damage: %d", displayTotalDamage), Vector2{ 50, 70 }, 24, 1, BLUE);
            }

            if (totalQuestions == 0) {
                DrawTextEx(SemiBold, TextFormat("Player Questions: ---"), Vector2{ 50, 100 }, 24, 1, BLUE);
            }
            else {
                DrawTextEx(SemiBold, TextFormat("Player Questions: %d", totalQuestions), Vector2{ 50, 100 }, 24, 1, BLUE);
            }

            if (p.getBlock() == 0) {
                DrawTextEx(SemiBold, TextFormat("Player Block: --- "), Vector2{ 50, 130 }, 24, 1, BLUE);
            }
            else {
                DrawTextEx(SemiBold, TextFormat("Player Block: %d", p.getBlock()), Vector2{ 50, 130 }, 24, 1, BLUE);
            }

            DrawTextEx(SemiBold, TextFormat("%s HP: %d / %d", e.getName().c_str(), e.getHp(), e.getMaxHp()), Vector2{ 900, 40 }, 24, 1, RED);

            if (damgeText == "0") {
                DrawTextEx(SemiBold, TextFormat("%s ATK: ---", e.getName().c_str()), Vector2{ 900, 70 }, 24, 1, RED);
            }
            else {
                DrawTextEx(SemiBold, TextFormat("%s ATK: %s", e.getName().c_str(), damgeText.c_str()), Vector2{ 900, 70 }, 24, 1, RED);
            }

            DrawTextEx(SemiBold, TextFormat("%s Intent: %s", e.getName().c_str(), enemyIntent.c_str()), Vector2{ 900, 100 }, 24, 1, RED);

            if (e.getBlock() == 0) {
                DrawTextEx(SemiBold, TextFormat("%s Block: ---", e.getName().c_str()), Vector2{ 900, 130 }, 24, 1, RED);
            }
            else {
                DrawTextEx(SemiBold, TextFormat("%s Block: %d", e.getName().c_str(), e.getBlock()), Vector2{ 900, 130 }, 24, 1, RED);
            }

            int logY = 10;
            float fixedLogX = 520.0f;

            DrawTextEx(Bold, "Combat Logs:", Vector2{ 520.0, 10 }, 20, 1, GRAY);

            int startLog;

            if (combatLogs.size() > 6) {
                startLog = combatLogs.size() - 6;
            }
            else {
                startLog = 0;
            }

            for (int i = startLog; i < combatLogs.size(); i++) {
                logY += 25;

                DrawTextEx(Bold, combatLogs[i].c_str(), Vector2{ fixedLogX, (float)logY }, 20, 1, LIGHTGRAY);
            }
        }

        if (currentState == STATE_PLAYER_TURN) {
            DrawTextEx(SemiBold, "Current State: [ PLAYER TURN ]", Vector2{ 320, 240 }, 32, 1, GOLD);
            DrawTextEx(SemiBold, "Press [ SPACE ] or click the button to end your turn", Vector2{ 320, 280 }, 26, 1, GRAY);

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

            drawDeck(hand, 490, Bold, p.getEnergy(), playerSelectedCard, false, virtualMousePos);

            viewDrawPileBtn.draw(SemiBold, Color{ 35, 45, 60, 255 }, Color{ 60, 80, 115, 255 }, Color{ 110, 125, 145, 255 }, virtualMousePos);
            viewDiscardPileBtn.draw(SemiBold, Color{ 35, 45, 60, 255 }, Color{ 60, 80, 115, 255 }, Color{ 110, 125, 145, 255 }, virtualMousePos);
            endTurnBtn.draw(Regular, Color{ 40, 40, 45, 255 }, Color{ 0, 185, 0, 255 }, Color{ 100, 100, 105, 255 }, virtualMousePos);

            for (int i = 0; i < (int)shuffleAnimations.size(); i++) {
                if (shuffleAnimations[i].isActive) {
                    drawShuffleCard(shuffleAnimations[i], Bold);
                }
            }

            if (!cardAnimations.empty() && cardAnimations[0].isActive) {
                drawMovingCard(cardAnimations[0], p.getEnergy(), Bold);
            }
        }
        else if (currentState == STATE_VIEW_PILE) {
            if (viewingDiscardPile) {
                viewPile(discardPile, SemiBold, virtualMousePos, "Discard Pile");
            }
            else {
                viewPile(drawPile, SemiBold, virtualMousePos, "Draw Pile");
            }

            backToTurnBtn.draw(SemiBold, Color{ 35, 45, 60, 255 }, Color{ 60, 80, 115, 255 }, Color{ 110, 125, 145, 255 }, virtualMousePos);
        }
        else if (currentState == STATE_MATH_QUIZ) {
            DrawTextEx(Regular, "Current State: [ MATH QUIZ ]", Vector2{ 320, 240 }, 32, 1, PURPLE);

            string questionFullText = TextFormat("Question: %s", currentQuiz.title.c_str());
            DrawTextEx(Regular, questionFullText.c_str(), Vector2{ 320, 280 }, 26, 1, WHITE);

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

            if (currentBarWidth < 0) {
                currentBarWidth = 0;
            }

            DrawRectangleLines(320, timeBarY, maxBarWidth, 15, GRAY);

            Color barColor = GREEN;
            float timeRatio = quizTimer / 10.0f;

            if (timeRatio > 1.0f) {
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

            DrawRectangle(320, timeBarY, currentBarWidth, 15, barColor);

            DrawTextEx(SemiBold, TextFormat("Total Damage Dealt: %d", turnFinalCalculatedDamage), Vector2{ 750, timeBarY - 3.0f }, 22, 1, RED);

            float skipBtnY = timeBarY + 25.0f;
            skipBtn.rect.y = skipBtnY;

            if (quizTimer > 10.0f) {
                skipBtn.text1 = "Continue";
            }
            else {
                skipBtn.text1 = "Skip";
            }

            skipBtn.draw(Regular, Color{ 60, 60, 60, 255 }, Color{ 185, 0, 0, 255 }, WHITE, virtualMousePos);

            if (quizTimer > 10.0f) {
                DrawTextEx(SemiBold, "TIME'S UP! Press Space or Click Continue.", Vector2{ 490, skipBtn.rect.y + 10 }, 20, 1, RED);
            }
            else {
                DrawTextEx(SemiBold, TextFormat("Time Left: %.1fs", 10.0f - quizTimer), Vector2{ 490, skipBtn.rect.y + 10 }, 20, 1, ORANGE);
            }

            int backLogQuestions = totalQuestions;

            if (totalQuestions > 0) {
                backLogQuestions = totalQuestions - 1;
            }
            else {
                backLogQuestions = 0;
            }

            DrawTextEx(SemiBold, TextFormat("Quizzes Left: %d", backLogQuestions), Vector2{ 320, skipBtn.rect.y + 55.0f }, 22, 1, BEIGE);
        }
        else if (currentState == STATE_ENEMY_TURN) {
            DrawTextEx(Regular, "Current State: [ ENEMY TURN ]", Vector2{ 320, 240 }, 32, 1, RED);
        }
        else if (currentState == STATE_GAME_OVER) {
            if (e.getHp() <= 0) {
                DrawTextEx(Regular, "You win!", Vector2{ 320, 240 }, 32, 1, RED);
            }

            if (p.getHp() <= 0) {
                DrawTextEx(Regular, "You lose! Game Over", Vector2{ 320, 240 }, 32, 1, RED);
            }
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(target.texture, Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, Rectangle{ (currentScreenWidth - (virtualScreenWidth * scale)) * 0.5f, (currentScreenHeight - (virtualScreenHeight * scale)) * 0.5f, virtualScreenWidth * scale, virtualScreenHeight * scale }, Vector2{ 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadFont(Thin);
    UnloadFont(Regular);
    UnloadFont(SemiBold);
    UnloadFont(Bold);
    UnloadImage(icon);
    CloseWindow();
}