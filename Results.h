#pragma once

#include <list>
#include <string>
#include <utility>

class Results {
public:
    enum EventType {
        LEVEL_TRANSITION = 1, // moved to another screen + which screen
        LIFE_LOST = 2,        // player lost a life
        RIDDLE_RESULT = 3,    // riddle details + answer + correct/wrong
        GAME_FINISHED = 4     // game ended + score
    };

    struct Event {
        size_t iteration;
        EventType type;
        std::string data;
    };

private:

public:
    std::list<Event> events;

    static Results loadResults(const std::string& filename);
    void saveResults(const std::string& filename) const;
    bool isEqual(const Results& other) const;

    void addEvent(size_t iteration, EventType type, const std::string& extraData = "") {
        events.push_back({ iteration, type, extraData });
    }
    bool isEmpty() const { return events.empty(); }
};