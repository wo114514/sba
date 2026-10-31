#include "QuestionGenerator.h"
#include "raylib.h"
#include <vector>

QuestionGenerator::QuestionGenerator() {}  
QuizInfo QuestionGenerator::generateAlgebraQuestion(int difficulty) {
    
    int questionType = GetRandomValue(1, 2); 
    int correctVal = 0;
    int wrong1 = 0;

    int A = 0;
    int B = 0;

    if (difficulty == 1) {
        A = GetRandomValue(0, 5);
        B = GetRandomValue(1, 5);
    } 
    else if (difficulty == 2) {
        A = GetRandomValue(-5, 5);
        B = GetRandomValue(1, 5);
    } 
    else {
        A = GetRandomValue(-10, 10);
        B = GetRandomValue(1, 10);
    }

    if (questionType == 1) {
        // A y - (-B y) = (A + B)y
        correctVal = A + B;
        wrong1 = A - B;
    } 
    else {
        // A y - (B y) = (A - B)y
        correctVal = A - B;
        wrong1 = A + B; 
    }
    
    while (wrong1 == correctVal) {
        wrong1 = GetRandomValue(-25, 25); 
    }

    int wrong2 = correctVal - GetRandomValue(1, 10);
    while (wrong2 == correctVal || wrong2 == wrong1) {
        wrong2 = correctVal - GetRandomValue(1, 10);
    }

    int wrong3 = correctVal + GetRandomValue(1, 10);
    while (wrong3 == correctVal || wrong3 == wrong1 || wrong3 == wrong2) {
        wrong3 = correctVal + GetRandomValue(1, 10);
    }

    int targetPos = GetRandomValue(1, 4);
    QuizInfo quiz;
    int lengthType = GetRandomValue(1, 3); // 1 = Short, 2 = Medium, 3 = Long
    
    if (lengthType == 1) {
        if (questionType == 1) {
            quiz.title = TextFormat("Simplify: %d y - (-%d y) = ?", A, B);
        } 
        else {
            quiz.title = TextFormat("Simplify: %d y - (%d y) = ?", A, B);
        }
    } 
    else if (lengthType == 2) {
        if (questionType == 1) {
            quiz.title = TextFormat("Advanced Algebra Problem (Turn %d):\nSimplify the basic linear expression: %d y - (-%d y) = ?", GetRandomValue(1, 5), A, B);
        } 
        else {
            quiz.title = TextFormat("Advanced Algebra Problem (Turn %d):\nSimplify the basic linear expression: %d y - (%d y) = ?", GetRandomValue(1, 5), A, B);
        }
    } 
    else {
        if (questionType == 1) {
            quiz.title = TextFormat("--- [MISSION CHALLENGE] ---\nGiven that y is a real variable and coefficients are non-zero.\nPlease calculate and simplify: %d y - (-%d y) = ?", A, B);
        } 
        else {
            quiz.title = TextFormat("--- [MISSION CHALLENGE] ---\nGiven that y is a real variable and coefficients are non-zero.\nPlease calculate and simplify: %d y - (%d y) = ?", A, B);
        }
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
