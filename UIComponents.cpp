#include "UIComponents.h"
#include <vector>
#include <string>
#include "raylib.h"
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
