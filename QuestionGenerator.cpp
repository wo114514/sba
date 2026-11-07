#include "QuestionGenerator.h"
#include "raylib.h"
#include <vector>

QuestionGenerator::QuestionGenerator() {}  
QuizInfo QuestionGenerator::generateAlgebraQuestion(int difficulty) {
    
    int questionType = GetRandomValue(1, 3); 
    int correctValue = 0;
    int wrong1 = 0;

    int A = 0;
    int B = 0;

    if (difficulty == 1) {
        A = GetRandomValue(0, 10);
        B = GetRandomValue(1, 10);
    } 
    else if (difficulty == 2) {
        A = GetRandomValue(-10, 10);
        B = GetRandomValue(1, 20);
    } 
    else {
        A = GetRandomValue(-20, 20);
        B = GetRandomValue(1, 30);
    }

    if (questionType == 1) {
        // A y - (-B y) = (A + B)y
        correctValue = A + B;
        wrong1 = A - B;
    } 
    else if (questionType == 2) {
        // A y + (-B y) = (A - B)y
        correctValue = A - B;
        wrong1 = A + B;
    } 
    else {
        // A y - (B y) = (A - B)y
        correctValue = A - B;
        wrong1 = A + B; 
    }
    
    while (wrong1 == correctValue) {
        wrong1 = GetRandomValue(-50, 50); 
    }

    int wrong2 = correctValue - GetRandomValue(1, 15);
    while (wrong2 == correctValue || wrong2 == wrong1) {
        wrong2 = correctValue - GetRandomValue(1, 15);
    }

    int wrong3 = correctValue + GetRandomValue(1, 15);
    while (wrong3 == correctValue || wrong3 == wrong1 || wrong3 == wrong2) {
        wrong3 = correctValue + GetRandomValue(1, 15);
    }

    int targetPos = GetRandomValue(1, 4);
    QuizInfo quiz;
    // 1 = Short, 2 = Medium, 3 = Long
    int lengthType = GetRandomValue(1, 3); 
    
    if (lengthType == 1) {
        if (questionType == 1) {
            quiz.title = TextFormat("Simplify: %d y - (-%d y) = ?", A, B);
        } 
        else if (questionType == 2) {
            quiz.title = TextFormat("Simplify: %d y + (-%d y) = ?", A, B);
        } 
        else {
            quiz.title = TextFormat("Simplify: %d y - (%d y) = ?", A, B);
        }
    } 
    else if (lengthType == 2) {
        if (questionType == 1) {
            quiz.title = TextFormat("New Questions!\nSimplify the expression: %d y - (-%d y) = ?", A, B);
        } 
        else if (questionType == 2) {
            quiz.title = TextFormat("New Questions!\nSimplify the expression: %d y + (-%d y) = ?", A, B);
        } 
        else {
            quiz.title = TextFormat("New Questions!\nSimplify the expression: %d y - (%d y) = ?", A, B);
        }
    } 
    else {
        if (questionType == 1) {
            quiz.title = TextFormat("Advanced Questions!\nGiven that y is a real variable and coefficients are non-zero.\nPlease simplify: %d y - (-%d y) = ?", A, B);
        } 
        else if (questionType == 2) {
            quiz.title = TextFormat("Advanced Questions!\nGiven that y is a real variable and coefficients are non-zero.\nPlease simplify: %d y + (-%d y) = ?", A, B);
        } 
        else {
            quiz.title = TextFormat("Advanced Questions!\nGiven that y is a real variable and coefficients are non-zero.\nPlease simplify: %d y - (%d y) = ?", A, B);
        }
    }

    if (targetPos == 1) {
        quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", correctValue, wrong1, wrong2, wrong3);
        quiz.optA = TextFormat("A. %dy", correctValue);
        quiz.optB = TextFormat("B. %dy", wrong1);
        quiz.optC = TextFormat("C. %dy", wrong2);
        quiz.optD = TextFormat("D. %dy", wrong3);
        quiz.correctAns = 'A';
    } 
    else if (targetPos == 2) {
        quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, correctValue, wrong2, wrong3);
        quiz.optA = TextFormat("A. %dy", wrong1);
        quiz.optB = TextFormat("B. %dy", correctValue);
        quiz.optC = TextFormat("C. %dy", wrong2);
        quiz.optD = TextFormat("D. %dy", wrong3);
        quiz.correctAns = 'B';
    } 
    else if (targetPos == 3) {
        quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, wrong2, correctValue, wrong3);
        quiz.optA = TextFormat("A. %dy", wrong1);
        quiz.optB = TextFormat("B. %dy", wrong2);
        quiz.optC = TextFormat("C. %dy", correctValue);
        quiz.optD = TextFormat("D. %dy", wrong3);
        quiz.correctAns = 'C';
    } 
    else {
        quiz.options = TextFormat("A. %dy B. %dy C. %dy D. %dy", wrong1, wrong2, wrong3, correctValue);
        quiz.optA = TextFormat("A. %dy", wrong1);
        quiz.optB = TextFormat("B. %dy", wrong2);
        quiz.optC = TextFormat("C. %dy", wrong3);
        quiz.optD = TextFormat("D. %dy", correctValue);
        quiz.correctAns = 'D';
    }

    return quiz;
}