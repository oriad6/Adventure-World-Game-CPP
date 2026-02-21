#include "Steps.h"
#include <fstream>

Steps Steps::loadSteps(const std::string& filename) {
    Steps steps;
    std::ifstream file(filename);
    if (!file.is_open()) return steps;

    long seed;
    file >> seed;
    steps.setRandomSeed(seed); 

    size_t count = 0;
    file >> count;

    for (size_t i = 0; i < count; ++i) {
        size_t iter;
        char key;
        file >> iter >> key;
        steps.addStep(iter, key);
    }

    file.close();
    return steps;
}

void Steps::saveSteps(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    file << randomSeed << std::endl;
    file << steps.size() << std::endl;
    for (const auto& step : steps) {
        file << step.first << " " << step.second << " ";
    }
    file.close();
}