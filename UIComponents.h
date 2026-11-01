#ifndef UIComponents_H
#define UIComponents_H
#include "raylib.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct gameButton {
    Rectangle rect;
    string text1;
    string text2;
    string text3;
    bool isHovered;

    void init(float x, float y, float width, float height, string t1, string t2 = "", string t3 = "");
    void update(Vector2 mousePos);
    bool isClicked(Vector2 mousePos);
    void draw(Font font, Color baseColor, Color hoverColor, Color borderColor, Vector2 currentMousePos);
};

struct quizButton {
    Rectangle rect;
    string text;
    char choice; 
    bool isHovered;

    void init(char choiceOoptions);
    void update(Vector2 mousePos, const string& optionText, float posX, float posY, int fontSize, Font font);
    void draw(int fontSize, Font font);
};

int drawDeck(
    const vector<int>& deck,
    int startY,
    Font font,
    int playerEnergy,
    int selectedCardIdx,
    bool isUpdate,
    Vector2 currentMousePos
);

void drawPileView(const vector<int>& pile, Font font, Vector2 currentMousePos, const string& title);

#endif