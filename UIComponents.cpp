#include "UIComponents.h"
#include <cmath>
#include <vector>
#include <string>
#include "raylib.h"
#include "Card.h"
#include <iostream>
using namespace std;

void gameButton::init(float x, float y, float width, float height, string t1, string t2, string t3) {
    rect = { x, y, width, height };
    text1 = t1;
    text2 = t2;
    text3 = t3;
    isHovered = false;
}
void gameButton::update(Vector2 mousePos) {
    isHovered = CheckCollisionPointRec(mousePos, rect);
}
bool gameButton::isClicked(Vector2 mousePos) {
    return isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
void gameButton::draw(Font font, Color baseColor, Color hoverColor, Color borderColor, Vector2 currentMousePos) {
    update(currentMousePos);
    Color currentBg;
    
    if (isHovered) {
        currentBg = hoverColor;
    } 
    else {
        currentBg = baseColor;
    }
    
    DrawRectangleRec(rect, currentBg);
    DrawRectangleLinesEx(rect, 1, borderColor);

    if (text2 == "" && text3 == "") {
        Vector2 textSize = MeasureTextEx(font, text1.c_str(), 20, 1);
        float tx = rect.x + (rect.width - textSize.x) / 2.0f;
        float ty = rect.y + (rect.height - textSize.y) / 2.0f;
        DrawTextEx(font, text1.c_str(), Vector2{tx, ty}, 20, 1, WHITE);
    } 
    else {
        DrawTextEx(font, text1.c_str(), Vector2{ rect.x + 18, rect.y + 10 }, 20, 1, WHITE);
        if (text2 != ""){
            DrawTextEx(font, text2.c_str(), Vector2{ rect.x + 18, rect.y + 32 }, 20, 1, WHITE);
        }
        if (text3 != ""){
            DrawTextEx(font, text3.c_str(), Vector2{ rect.x + 18, rect.y + 54 }, 20, 1, WHITE);
        }
    }
}

void quizButton::init(char choiceOoptions) {
    choice = choiceOoptions;
    rect = { 0, 0, 0, 0 };
    text = "";
    isHovered = false;
}
void quizButton::update(Vector2 mousePos, const string& optionText, float posX, float posY, int fontSize, Font font) {
    text = optionText;
    int textWidth = MeasureTextEx(font, text.c_str(), fontSize, 1.0f).x; 
    rect = { posX, posY, (float)textWidth + 30, (float)fontSize + 20 };
    isHovered = CheckCollisionPointRec(mousePos, rect);
}
void quizButton::draw(int fontSize, Font font) {
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

struct CardAnimation {
    float hoverOffset;
    float scale;
};

void updateCardAnimation(
    CardAnimation& animation,
    bool isActive
) {
    float targetOffset = 0.0f;
    float targetScale = 1.0f;

    if (isActive) {
        targetOffset = 40.0f;
        targetScale = 1.12f;
    }

    float animationSpeed = 8.0f * GetFrameTime();

    if (animationSpeed > 1.0f) {
        animationSpeed = 1.0f;
    }

    animation.hoverOffset +=
        (targetOffset - animation.hoverOffset) * animationSpeed;

    animation.scale +=
        (targetScale - animation.scale) * animationSpeed;
}

// Draw a single card
void drawSingleCard(
    Card currentCard,
    float posX,
    float currentCardY,
    float cardWidth,
    float cardHeight,
    float scale,
    int cardIndex,
    int playerEnergy,
    bool isHovered,
    bool isSelected,
    Font font
) {
    // Scale the card around its center
    float drawX = posX - (cardWidth * (scale - 1.0f) / 2.0f);
    float drawY = currentCardY - (cardHeight * (scale - 1.0f) / 2.0f);

    float drawWidth = cardWidth * scale;
    float drawHeight = cardHeight * scale;

    Color baseColor = Color{ 30, 30, 40, 255 };
    Color borderColor = GRAY;

    if (playerEnergy < currentCard.cost) {
        borderColor = RED;
    }
    else if (isHovered || isSelected) {
        borderColor = GOLD;
    }

    if (isHovered || isSelected) {
        baseColor = Color{ 50, 50, 70, 255 };
    }

    Rectangle cardRect = {
    drawX, drawY, drawWidth, drawHeight
    };

    // Draw card shadow
    Rectangle shadowRect = {
        drawX + 4.0f * scale,
        drawY + 5.0f * scale,
        drawWidth,
        drawHeight
    };

    DrawRectangleRounded(
        shadowRect,
        0.08f,
        8,
        Color{ 0, 0, 0, 110 }
    );

    // Draw outer card border
    DrawRectangleRounded(
        cardRect,
        0.08f,
        8,
        borderColor
    );

    // Draw inner card background
    Rectangle innerRect = {
        drawX + 2.0f * scale,
        drawY + 2.0f * scale,
        drawWidth - 4.0f * scale,
        drawHeight - 4.0f * scale
    };

    DrawRectangleRounded(
        innerRect,
        0.07f,
        8,
        baseColor
    );

    // Draw energy cost
    float energyCircleX = drawX + 10.0f * scale;
    float energyCircleY = drawY;

    // Draw energy circle shadow
    DrawCircle(
        energyCircleX,
        energyCircleY + 2.0f * scale,
        17.0f * scale,
        Color{ 0, 0, 0, 120 }
    );

    // Draw energy circle
    DrawCircle(
        energyCircleX,
        energyCircleY,
        16.0f * scale,
        ORANGE
    );

    // Draw energy circle border
    DrawCircleLines(
        energyCircleX,
        energyCircleY,
        16.0f * scale,
        WHITE
    );

    DrawTextEx(
        font,
        TextFormat("%d", currentCard.cost),
        Vector2{
            energyCircleX - 7.0f * scale,
            energyCircleY - 12.0f * scale
        },
        26.0f * scale,
        1.0f,
        WHITE
    );

    float textAlignX = drawX + 15.0f * scale;

    // Draw card name
    string cardName = currentCard.name;
    int splitPos = (int)cardName.find_first_of("- ");
    float nameFontSize = 22.0f * scale;

    if (splitPos != -1) {
        string firstLine = cardName.substr(0, splitPos);
        string secondLine = cardName.substr(splitPos + 1);

        DrawTextEx(font, firstLine.c_str(), Vector2{ textAlignX, drawY + 24.0f * scale }, nameFontSize, 1, WHITE);

        DrawTextEx(
            font,
            secondLine.c_str(),
            Vector2{ textAlignX, drawY + 46.0f * scale },
            nameFontSize,
            1,
            WHITE
        );
    }
    else {
        DrawTextEx(
            font,
            cardName.c_str(),
            Vector2{ textAlignX, drawY + 24.0f * scale },
            22.0f * scale,
            1,
            WHITE
        );
    }

    // Draw attack
    if (currentCard.damage > 0) {
        DrawTextEx(
            font,
            TextFormat("ATK: %d", currentCard.damage),
            Vector2{ textAlignX, drawY + 77.0f * scale },
            22.0f * scale,
            1,
            RED
        );
    }

    // Draw block
    if (currentCard.block > 0) {
        float blockYOffset = 77.0f;

        if (currentCard.damage > 0) {
            blockYOffset += 22.0f;
        }

        DrawTextEx(
            font,
            TextFormat("DEF: %d", currentCard.block),
            Vector2{ textAlignX, drawY + blockYOffset * scale },
            22.0f * scale,
            1,
            BLUE
        );
    }

    // Draw card draw number
    if (currentCard.drawNumber > 0) {
        float drawYOffset = 77.0f;

        if (currentCard.damage > 0) {
            drawYOffset += 22.0f;
        }

        if (currentCard.block > 0) {
            drawYOffset += 22.0f;
        }

        DrawTextEx(
            font,
            TextFormat("DRAW: %d", currentCard.drawNumber),
            Vector2{ textAlignX, drawY + drawYOffset * scale },
            22.0f * scale,
            1,
            GOLD
        );
    }

    // Draw gained energy
    if (currentCard.energyGained > 0) {
        float energyGainedYOffset = 77.0f;

        if (currentCard.damage > 0) {
            energyGainedYOffset += 22.0f;
        }

        if (currentCard.block > 0) {
            energyGainedYOffset += 22.0f;
        }

        if (currentCard.drawNumber > 0) {
            energyGainedYOffset += 22.0f;
        }

        DrawTextEx(
            font,
            TextFormat("ENG: +%d", currentCard.energyGained),
            Vector2{ textAlignX, drawY + energyGainedYOffset * scale },
            22.0f * scale,
            1,
            ORANGE
        );
    }

    // Draw card index
    DrawTextEx(
        font,
        TextFormat("#%d", cardIndex + 1),
        Vector2{
            drawX + (cardWidth - 34.0f) * scale,
            drawY + (cardHeight - 22.0f) * scale
        },
        16.0f * scale,
        1,
        WHITE
    );
}

void drawPileView(const vector<int>& pile, Font font, Vector2 currentMousePos) {

    // Draw background
    DrawRectangle(0, 0, 1280, 720, Color{ 20, 27, 40, 255 });

    // Draw title
    string title = "Draw Pile";
    float titleSize = 36.0f;
    float titleWidth = MeasureTextEx(font, title.c_str(), titleSize, 1).x;

    DrawTextEx(
        font,
        title.c_str(),
        Vector2{ (1280 - titleWidth) / 2, 25 },
        titleSize,
        1,
        WHITE
    );

    // Draw card count
    string countText = "Cards: " + to_string(pile.size());
    float countWidth = MeasureTextEx(font, countText.c_str(), 20, 1).x;

    DrawTextEx(
        font,
        countText.c_str(),
        Vector2{ (1280 - countWidth) / 2, 70 },
        20,
        1,
        LIGHTGRAY
    );

    // Card grid settings
    int columns = 6;

    float cardWidth = 145.0f;
    float cardHeight = 180.0f;

    float horizontalGap = 20.0f;
    float verticalGap = 25.0f;

    float totalWidth =
        columns * cardWidth + (columns - 1) * horizontalGap;

    float startX = (1280 - totalWidth) / 2;
    float startY = 115.0f;

    // Prepare card animations
    static vector<CardAnimation> animations;
    static vector<int> previousPile;

    if (previousPile != pile) {
        animations.clear();
        animations.resize(pile.size());

        for (int i = 0; i < (int)animations.size(); i++) {
            animations[i].hoverOffset = 0.0f;
            animations[i].scale = 1.0f;
        }

        previousPile = pile;
    }

    // Find hovered card
    int hoveredIndex = -1;

    for (int i = 0; i < (int)pile.size(); i++) {

        int row = i / columns;
        int column = i % columns;

        float posX = startX + column * (cardWidth + horizontalGap);
        float posY = startY + row * (cardHeight + verticalGap);

        Rectangle cardRect = {
            posX,
            posY,
            cardWidth,
            cardHeight
        };

        if (CheckCollisionPointRec(currentMousePos, cardRect)) {
            hoveredIndex = i;
        }
    }

    // Update animation
    for (int i = 0; i < (int)animations.size(); i++) {
        updateCardAnimation(animations[i], i == hoveredIndex);
    }

    // Draw normal cards first
    for (int i = 0; i < (int)pile.size(); i++) {

        if (i == hoveredIndex) {
            continue;
        }

        int row = i / columns;
        int column = i % columns;

        float posX = startX + column * (cardWidth + horizontalGap);
        float posY = startY + row * (cardHeight + verticalGap);

        int cardIndex = (int)pile.size() - 1 - i;
        Card currentCard = CARD_DATABASE[pile[cardIndex]];

        drawSingleCard(
            currentCard,
            posX,
            posY - animations[i].hoverOffset,
            cardWidth,
            cardHeight,
            animations[i].scale,
            i,
            999,
            false,
            false,
            font
        );
    }

    // Draw hovered card last
    if (hoveredIndex != -1) {

        int row = hoveredIndex / columns;
        int column = hoveredIndex % columns;

        float posX = startX + column * (cardWidth + horizontalGap);
        float posY = startY + row * (cardHeight + verticalGap);

        int cardIndex = (int)pile.size() - 1 - hoveredIndex;
        Card currentCard = CARD_DATABASE[pile[cardIndex]];

        drawSingleCard(
            currentCard,
            posX,
            posY - animations[hoveredIndex].hoverOffset,
            cardWidth,
            cardHeight,
            animations[hoveredIndex].scale,
            hoveredIndex,
            999,
            true,
            false,
            font
        );
    }
}

int drawDeck(
    const vector<int>& deck,
    int startY,
    Font font,
    int playerEnergy,
    int selectedCardIdx,
    bool isUpdate,
    Vector2 currentMousePos
) {
    // Store animation data between frames
    static vector<CardAnimation> animations;
    static vector<int> previousDeck;

    // Reset animation data when hand changes
    if (deck != previousDeck) {
        animations.clear();

        for (int i = 0; i < (int)deck.size(); i++) {
            CardAnimation animation;
            animation.hoverOffset = 0.0f;
            animation.scale = 1.0f;

            animations.push_back(animation);
        }

        previousDeck = deck;
    }

    if (deck.empty()) {
        int textWidth = MeasureTextEx(
            font, "Your hand is empty!", 26, 1
        ).x;

        DrawTextEx(
            font,
            "Your hand is empty!",
            Vector2{
                (1280.0f - textWidth) / 2.0f,
                (float)startY + 50
            },
            26, 1, DARKGRAY
        );

        return -1;
    }

    const float cardWidth = 140.0f;
    const float cardHeight = 180.0f;
    const float cardSpacing = 16.0f;

    int numCards = (int)deck.size();

    float totalDeckWidth =
        (numCards * cardWidth) +
        ((numCards - 1) * cardSpacing);

    float startX = (1280.0f - totalDeckWidth) / 2.0f;

    int clickedIndex = -1;

    // Store hover information
    vector<bool> hoverStates(numCards, false);

    // Update animation and detect mouse clicks
    for (int i = 0; i < numCards; i++) {
        float posX = startX + i * (cardWidth + cardSpacing);

        Rectangle baseCheckRect = {
            posX,
            (float)startY,
            cardWidth,
            cardHeight
        };

        bool isHovered = CheckCollisionPointRec(
            currentMousePos,
            baseCheckRect
        );

        bool isSelected = (i == selectedCardIdx);

        hoverStates[i] = isHovered;

        updateCardAnimation(
            animations[i],
            isHovered || isSelected
        );

        if (isUpdate &&
            isHovered &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

            clickedIndex = i;
        }
    }

    for (int i = 0; i < numCards; i++) {
        if (hoverStates[i] || i == selectedCardIdx) {
            continue;
        }

        int cardID = deck[i];
        Card currentCard = CARD_DATABASE[cardID];

        float posX = startX + i * (cardWidth + cardSpacing);

        float currentCardY =
            (float)startY - animations[i].hoverOffset;

        drawSingleCard(
            currentCard,
            posX,
            currentCardY,
            cardWidth,
            cardHeight,
            animations[i].scale,
            i,
            playerEnergy,
            false,
            false,
            font
        );
    }

    if (selectedCardIdx >= 0 && selectedCardIdx < numCards && !hoverStates[selectedCardIdx]) {

        int cardID = deck[selectedCardIdx];
        Card currentCard = CARD_DATABASE[cardID];

        float posX = startX +
            selectedCardIdx * (cardWidth + cardSpacing);

        float currentCardY =
            (float)startY - animations[selectedCardIdx].hoverOffset;

        drawSingleCard(
            currentCard,
            posX,
            currentCardY,
            cardWidth,
            cardHeight,
            animations[selectedCardIdx].scale,
            selectedCardIdx,
            playerEnergy,
            false,
            true,
            font
        );
    }

    for (int i = 0; i < numCards; i++) {
        if (!hoverStates[i]) {
            continue;
        }

        int cardID = deck[i];
        Card currentCard = CARD_DATABASE[cardID];

        float posX = startX + i * (cardWidth + cardSpacing);

        float currentCardY =
            (float)startY - animations[i].hoverOffset;

        drawSingleCard(
            currentCard,
            posX,
            currentCardY,
            cardWidth,
            cardHeight,
            animations[i].scale,
            i,
            playerEnergy,
            true,
            (i == selectedCardIdx),
            font
        );
    }

    return clickedIndex;
}