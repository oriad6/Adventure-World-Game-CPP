#pragma once
#include <string>
#include <iostream>

class Riddle {
public:
    static constexpr int NUM_OPTS = 4;
private:
    std::string question;
    std::string options[NUM_OPTS];
    char correctAnswer;

public:
    Riddle(const std::string& q, const std::string opts[NUM_OPTS], char ans);
    void display() const;
    bool isCorrect(char choice) const {
        return std::toupper(choice) == correctAnswer;
    }
    const std::string& getQuestion() const { return question; }
};