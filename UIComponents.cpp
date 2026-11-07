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
        DrawTextEx(font, text1.c_str(), Vector2{ tx, ty }, 20, 1, WHITE);
    }
    else {
        DrawTextEx(font, text1.c_str(), Vector2{ rect.x + 18, rect.y + 10 }, 20, 1, WHITE);

        if (text2 != "") {
            DrawTextEx(font, text2.c_str(), Vector2{ rect.x + 18, rect.y + 32 }, 20, 1, WHITE);
        }

        if (text3 != "") {
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
    DrawTextEx(font, text.c_str(), Vector2{ rect.x + 15, rect.y + 10 }, fontSize, 1, textColor);
}

struct CardAnimation {
    float hoverOffset;
    float scale;
};

void updateCardAnimation(CardAnimation& animation, bool isActive) {
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

    animation.hoverOffset += (targetOffset - animation.hoverOffset) * animationSpeed;
    animation.scale += (targetScale - animation.scale) * animationSpeed;
}

void drawSingleCard(Card currentCard, float posX, float currentCardY, float cardWidth, float cardHeight, float scale, int cardIndex, int playerEnergy, bool isHovered, bool isSelected, Font font) {
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

    Rectangle cardRect = { drawX, drawY, drawWidth, drawHeight };
    Rectangle shadowRect = { drawX + 4.0f * scale, drawY + 5.0f * scale, drawWidth, drawHeight };

    DrawRectangleRounded(shadowRect, 0.08f, 8, Color{ 0, 0, 0, 110 });
    DrawRectangleRounded(cardRect, 0.08f, 8, borderColor);

    Rectangle innerRect = { drawX + 2.0f * scale, drawY + 2.0f * scale, drawWidth - 4.0f * scale, drawHeight - 4.0f * scale };
    DrawRectangleRounded(innerRect, 0.07f, 8, baseColor);

    float energyCircleX = drawX + 10.0f * scale;
    float energyCircleY = drawY;

    DrawCircle(energyCircleX, energyCircleY + 2.0f * scale, 17.0f * scale, Color{ 0, 0, 0, 120 });
    DrawCircle(energyCircleX, energyCircleY, 16.0f * scale, ORANGE);
    DrawCircleLines(energyCircleX, energyCircleY, 16.0f * scale, WHITE);
    DrawTextEx(font, TextFormat("%d", currentCard.cost), Vector2{ energyCircleX - 7.0f * scale, energyCircleY - 12.0f * scale }, 26.0f * scale, 1.0f, WHITE);

    float textAlignX = drawX + 15.0f * scale;
    string cardName = currentCard.name;
    int splitPos = (int)cardName.find_first_of("- ");
    float nameFontSize = 22.0f * scale;

    if (splitPos != -1) {
        string firstLine = cardName.substr(0, splitPos);
        string secondLine = cardName.substr(splitPos + 1);

        DrawTextEx(font, firstLine.c_str(), Vector2{ textAlignX, drawY + 24.0f * scale }, nameFontSize, 1, WHITE);
        DrawTextEx(font, secondLine.c_str(), Vector2{ textAlignX, drawY + 46.0f * scale }, nameFontSize, 1, WHITE);
    }
    else {
        DrawTextEx(font, cardName.c_str(), Vector2{ textAlignX, drawY + 24.0f * scale }, 22.0f * scale, 1, WHITE);
    }

    if (currentCard.damage > 0) {
        DrawTextEx(font, TextFormat("ATK: %d", currentCard.damage), Vector2{ textAlignX, drawY + 77.0f * scale }, 22.0f * scale, 1, RED);
    }

    if (currentCard.block > 0) {
        float blockYOffset = 77.0f;

        if (currentCard.damage > 0) {
            blockYOffset += 22.0f;
        }

        DrawTextEx(font, TextFormat("DEF: %d", currentCard.block), Vector2{ textAlignX, drawY + blockYOffset * scale }, 22.0f * scale, 1, BLUE);
    }

    if (currentCard.drawNumber > 0) {
        float drawYOffset = 77.0f;

        if (currentCard.damage > 0) {
            drawYOffset += 22.0f;
        }

        if (currentCard.block > 0) {
            drawYOffset += 22.0f;
        }

        DrawTextEx(font, TextFormat("DRAW: %d", currentCard.drawNumber), Vector2{ textAlignX, drawY + drawYOffset * scale }, 22.0f * scale, 1, GOLD);
    }

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

        DrawTextEx(font, TextFormat("ENG: +%d", currentCard.energyGained), Vector2{ textAlignX, drawY + energyGainedYOffset * scale }, 22.0f * scale, 1, ORANGE);
    }

    DrawTextEx(font, TextFormat("#%d", cardIndex + 1), Vector2{ drawX + (cardWidth - 34.0f) * scale, drawY + (cardHeight - 22.0f) * scale }, 16.0f * scale, 1, WHITE);
}

void viewPile(const vector<int>& pile, Font font, Vector2 currentMousePos, const string& title) {
    DrawRectangle(0, 0, 1280, 720, Color{ 20, 27, 40, 255 });

    float titleSize = 36.0f;
    float titleWidth = MeasureTextEx(font, title.c_str(), titleSize, 1).x;
    DrawTextEx(font, title.c_str(), Vector2{ (1280 - titleWidth) / 2, 25 }, titleSize, 1, WHITE);

    string countText = "Cards: " + to_string(pile.size());
    float countWidth = MeasureTextEx(font, countText.c_str(), 20, 1).x;
    DrawTextEx(font, countText.c_str(), Vector2{ (1280 - countWidth) / 2, 70 }, 20, 1, LIGHTGRAY);

    int columns = 6;
    float cardWidth = 130.0f;
    float cardHeight = 180.0f;
    float horizontalGap = 20.0f;
    float verticalGap = 45.0f;
    float startY = 175.0f;
    float viewHeight = 420.0f;
    float totalWidth = columns * cardWidth + (columns - 1) * horizontalGap;
    float startX = (1280 - totalWidth) / 2;

    float scrollBarX = 1170.0f;
    float scrollBarY = startY;
    float scrollBarWidth = 14.0f;
    float scrollBarHeight = viewHeight;

    int totalRows = (int)pile.size() / columns;

    if (pile.size() % columns != 0) {
        totalRows++;
    }

    float contentHeight = 0.0f;

    if (totalRows > 0) {
        contentHeight = totalRows * cardHeight + (totalRows - 1) * verticalGap;
    }

    float maxScroll = contentHeight - viewHeight;

    if (maxScroll < 0) {
        maxScroll = 0;
    }

    static float scrollOffset = 0.0f;
    static bool isDraggingScrollbar = false;
    static float dragOffsetY = 0.0f;

    Rectangle scrollViewRect = { startX - 25, startY, totalWidth + 50, viewHeight };

    if (CheckCollisionPointRec(currentMousePos, scrollViewRect)) {
        float wheelMove = GetMouseWheelMove();
        scrollOffset -= wheelMove * 45.0f;
    }

    if (scrollOffset > maxScroll) {
        scrollOffset = maxScroll;
    }

    if (scrollOffset < 0) {
        scrollOffset = 0;
    }

    float thumbHeight = scrollBarHeight;

    if (contentHeight > 0) {
        thumbHeight = scrollBarHeight * (viewHeight / contentHeight);
    }

    if (thumbHeight < 40.0f) {
        thumbHeight = 40.0f;
    }

    if (thumbHeight > scrollBarHeight) {
        thumbHeight = scrollBarHeight;
    }

    float thumbY = scrollBarY;

    if (maxScroll > 0) {
        thumbY += (scrollOffset / maxScroll) * (scrollBarHeight - thumbHeight);
    }

    Rectangle thumbRect = { scrollBarX, thumbY, scrollBarWidth, thumbHeight };
    Rectangle trackRect = { scrollBarX, scrollBarY, scrollBarWidth, scrollBarHeight };

    if (maxScroll > 0) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(currentMousePos, trackRect)) {
            if (CheckCollisionPointRec(currentMousePos, thumbRect)) {
                isDraggingScrollbar = true;
                dragOffsetY = currentMousePos.y - thumbY;
            }
            else {
                float newThumbY = currentMousePos.y - thumbHeight / 2.0f;

                if (newThumbY < scrollBarY) {
                    newThumbY = scrollBarY;
                }

                if (newThumbY > scrollBarY + scrollBarHeight - thumbHeight) {
                    newThumbY = scrollBarY + scrollBarHeight - thumbHeight;
                }

                scrollOffset = ((newThumbY - scrollBarY) / (scrollBarHeight - thumbHeight)) * maxScroll;

                isDraggingScrollbar = true;
                dragOffsetY = thumbHeight / 2.0f;
            }
        }

        if (isDraggingScrollbar) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                float newThumbY = currentMousePos.y - dragOffsetY;

                if (newThumbY < scrollBarY) {
                    newThumbY = scrollBarY;
                }

                if (newThumbY > scrollBarY + scrollBarHeight - thumbHeight) {
                    newThumbY = scrollBarY + scrollBarHeight - thumbHeight;
                }

                scrollOffset = ((newThumbY - scrollBarY) / (scrollBarHeight - thumbHeight)) * maxScroll;
            }
            else {
                isDraggingScrollbar = false;
            }
        }
    }
    else {
        isDraggingScrollbar = false;
        scrollOffset = 0;
    }

    thumbY = scrollBarY;

    if (maxScroll > 0) {
        thumbY += (scrollOffset / maxScroll) * (scrollBarHeight - thumbHeight);
    }

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

    int hoveredIndex = -1;

    Rectangle viewRect = { startX, startY, totalWidth, viewHeight };

    if (CheckCollisionPointRec(currentMousePos, viewRect)) {
        for (int i = 0; i < (int)pile.size(); i++) {
            int row = i / columns;
            int column = i % columns;

            float posX = startX + column * (cardWidth + horizontalGap);
            float posY = startY + row * (cardHeight + verticalGap) - scrollOffset;

            Rectangle cardRect = { posX, posY, cardWidth, cardHeight };

            if (CheckCollisionPointRec(currentMousePos, cardRect)) {
                hoveredIndex = i;
            }
        }
    }

    for (int i = 0; i < (int)animations.size(); i++) {
        updateCardAnimation(animations[i], i == hoveredIndex);
    }

    BeginScissorMode((int)(startX - 30), (int)(startY - 75), (int)(totalWidth + 60), (int)(viewHeight + 75));

    for (int i = 0; i < (int)pile.size(); i++) {
        if (i == hoveredIndex) {
            continue;
        }

        int row = i / columns;
        int column = i % columns;

        float posX = startX + column * (cardWidth + horizontalGap);
        float posY = startY + row * (cardHeight + verticalGap) - scrollOffset;

        int cardIndex = (int)pile.size() - 1 - i;
        Card currentCard = CARD_DATABASE[pile[cardIndex]];

        drawSingleCard(currentCard, posX, posY - animations[i].hoverOffset, cardWidth, cardHeight, animations[i].scale, i, 999, false, false, font);
    }

    if (hoveredIndex != -1) {
        int row = hoveredIndex / columns;
        int column = hoveredIndex % columns;

        float posX = startX + column * (cardWidth + horizontalGap);
        float posY = startY + row * (cardHeight + verticalGap) - scrollOffset;

        int cardIndex = (int)pile.size() - 1 - hoveredIndex;
        Card currentCard = CARD_DATABASE[pile[cardIndex]];

        drawSingleCard(currentCard, posX, posY - animations[hoveredIndex].hoverOffset, cardWidth, cardHeight, animations[hoveredIndex].scale, hoveredIndex, 999, true, false, font);
    }

    EndScissorMode();

    DrawRectangleRounded(trackRect, 0.5f, 8, Color{ 45, 55, 70, 255 });

    Rectangle finalThumbRect = { scrollBarX, thumbY, scrollBarWidth, thumbHeight };
    Color thumbColor = Color{ 125, 145, 170, 255 };

    if (isDraggingScrollbar || CheckCollisionPointRec(currentMousePos, finalThumbRect)) {
        thumbColor = Color{ 185, 200, 220, 255 };
    }

    DrawRectangleRounded(finalThumbRect, 0.5f, 8, thumbColor);
}

int drawDeck(const vector<int>& deck, int startY, Font font, int playerEnergy, int selectedCardIdx, bool isUpdate, Vector2 currentMousePos) {
    static vector<CardAnimation> animations;
    static vector<int> previousDeck;

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
        int textWidth = MeasureTextEx(font, "Your hand is empty!", 26, 1).x;

        DrawTextEx(font, "Your hand is empty!", Vector2{ (1280.0f - textWidth) / 2.0f, (float)startY + 50 }, 26, 1, DARKGRAY);

        return -1;
    }

    const float cardWidth = 130.0f;
    const float cardHeight = 180.0f;
    const float cardSpacing = 16.0f;

    int numCards = (int)deck.size();
    float totalDeckWidth = (numCards * cardWidth) + ((numCards - 1) * cardSpacing);
    float startX = (1280.0f - totalDeckWidth) / 2.0f;

    int clickedIndex = -1;

    vector<bool> hoverStates(numCards, false);

    for (int i = 0; i < numCards; i++) {
        float posX = startX + i * (cardWidth + cardSpacing);
        Rectangle baseCheckRect = { posX, (float)startY, cardWidth, cardHeight };

        bool isHovered = CheckCollisionPointRec(currentMousePos, baseCheckRect);
        bool isSelected = (i == selectedCardIdx);

        hoverStates[i] = isHovered;

        updateCardAnimation(animations[i], isHovered || isSelected);

        if (isUpdate && isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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
        float currentCardY = (float)startY - animations[i].hoverOffset;

        drawSingleCard(currentCard, posX, currentCardY, cardWidth, cardHeight, animations[i].scale, i, playerEnergy, false, false, font);
    }

    if (selectedCardIdx >= 0 && selectedCardIdx < numCards && !hoverStates[selectedCardIdx]) {
        int cardID = deck[selectedCardIdx];
        Card currentCard = CARD_DATABASE[cardID];

        float posX = startX + selectedCardIdx * (cardWidth + cardSpacing);
        float currentCardY = (float)startY - animations[selectedCardIdx].hoverOffset;

        drawSingleCard(currentCard, posX, currentCardY, cardWidth, cardHeight, animations[selectedCardIdx].scale, selectedCardIdx, playerEnergy, false, true, font);
    }

    for (int i = 0; i < numCards; i++) {
        if (!hoverStates[i]) {
            continue;
        }

        int cardID = deck[i];
        Card currentCard = CARD_DATABASE[cardID];

        float posX = startX + i * (cardWidth + cardSpacing);
        float currentCardY = (float)startY - animations[i].hoverOffset;

        drawSingleCard(currentCard, posX, currentCardY, cardWidth, cardHeight, animations[i].scale, i, playerEnergy, true, (i == selectedCardIdx), font);
    }

    return clickedIndex;
}

void drawMovingCard(CardMoveAnimation animation, int playerEnergy, Font font) {
    const float cardWidth = 140.0f;
    const float cardHeight = 180.0f;

    float currentX = animation.startPosition.x + (animation.targetPosition.x - animation.startPosition.x) * animation.progress;
    float currentY = animation.startPosition.y + (animation.targetPosition.y - animation.startPosition.y) * animation.progress;

    float scale;

    if (animation.isDiscarding) {
        scale = 1.0f - 0.65f * animation.progress;
    }
    else {
        scale = 0.35f + 0.65f * animation.progress;
    }

    Card currentCard = CARD_DATABASE[animation.cardID];

    drawSingleCard(currentCard, currentX, currentY, cardWidth, cardHeight, scale, 0, playerEnergy, false, false, font);
}

void drawShuffleCard(ShuffleCardAnimation animation, Font font) {
    const float cardWidth = 140.0f;
    const float cardHeight = 180.0f;

    float progress = animation.progress;

    if (progress < 0.0f) {
        progress = 0.0f;
    }

    if (progress > 1.0f) {
        progress = 1.0f;
    }

    float currentX = animation.startPosition.x + (animation.targetPosition.x - animation.startPosition.x) * progress;
    float currentY = animation.startPosition.y + (animation.targetPosition.y - animation.startPosition.y) * progress - animation.arcHeight * 4.0f * progress * (1.0f - progress);

    float smallWidth = cardWidth * 0.15f;
    float smallHeight = cardHeight * 0.15f;

    Vector2 cardCenter = { currentX + cardWidth / 2.0f, currentY + cardHeight / 2.0f };

    float trailStart = progress - 0.35f;

    if (trailStart < 0.0f) {
        trailStart = 0.0f;
    }

    const int trailSegments = 12;

    for (int i = 0; i < trailSegments; i++) {
        float t1 = trailStart + (progress - trailStart) * i / trailSegments;
        float t2 = trailStart + (progress - trailStart) * (i + 1) / trailSegments;

        float x1 = animation.startPosition.x + (animation.targetPosition.x - animation.startPosition.x) * t1 + cardWidth / 2.0f;
        float y1 = animation.startPosition.y + (animation.targetPosition.y - animation.startPosition.y) * t1 - animation.arcHeight * 4.0f * t1 * (1.0f - t1) + cardHeight / 2.0f;

        float x2 = animation.startPosition.x + (animation.targetPosition.x - animation.startPosition.x) * t2 + cardWidth / 2.0f;
        float y2 = animation.startPosition.y + (animation.targetPosition.y - animation.startPosition.y) * t2 - animation.arcHeight * 4.0f * t2 * (1.0f - t2) + cardHeight / 2.0f;

        float alpha = (float)(i + 1) / trailSegments;

        Color outerGlow = { 50, 150, 255, (unsigned char)(alpha * 90) };
        Color middleGlow = { 100, 200, 255, (unsigned char)(alpha * 150) };
        Color innerGlow = { 220, 245, 255, (unsigned char)(alpha * 220) };

        DrawLineEx({ x1, y1 }, { x2, y2 }, 36.0f, outerGlow);
        DrawLineEx({ x1, y1 }, { x2, y2 }, 22.0f, middleGlow);
        DrawLineEx({ x1, y1 }, { x2, y2 }, 8.0f, innerGlow);
    }

    Rectangle cardRect = { cardCenter.x - smallWidth / 2.0f, cardCenter.y - smallHeight / 2.0f, smallWidth, smallHeight };

    DrawRectangleRounded({ cardRect.x - 5.0f, cardRect.y - 5.0f, cardRect.width + 10.0f, cardRect.height + 10.0f }, 0.25f, 8, Fade(SKYBLUE, 0.45f));
    DrawRectangleRounded(cardRect, 0.2f, 8, RAYWHITE);
    DrawRectangleRoundedLinesEx(cardRect, 0.2f, 8, 2.0f, SKYBLUE);
}