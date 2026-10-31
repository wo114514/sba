#ifndef QUESTION_GENERATOR_H
#define QUESTION_GENERATOR_H

#include <iostream>
using namespace std;

struct QuizInfo {
    string title;
    string options; // Options
    string optA;    // Stores text A
    string optB;    // Stores text B
    string optC;    // Stores text C
    string optD;    // Stores text D
    char correctAns;
};

class QuestionGenerator {
public:
    QuestionGenerator();
    QuizInfo generateAlgebraQuestion();   
};

#endif