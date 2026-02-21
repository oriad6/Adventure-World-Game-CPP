#pragma once

#include <list>
#include <string>
#include <utility>

class Steps {
  long randomSeed = 0;
  std::list<std::pair<size_t, char>> steps;

public:
  static Steps loadSteps(const std::string &filename);
  void saveSteps(const std::string &filename) const;

  void setRandomSeed(long seed) { randomSeed = seed; }
  long getRandomSeed() const { return randomSeed; }
  char peekStep(size_t iteration) const {
      if (hasStepAt(iteration)) 
          return steps.front().second;
      return 0;
  }
  void addStep(size_t iteration, char key) {
    steps.push_back({iteration, key});
  }

  bool hasStepAt(size_t iteration) const {
    return !steps.empty() && steps.front().first == iteration;
  }

  char popStep() {
    char key = steps.front().second;
    steps.pop_front();
    return key;
  }

  char getStep(size_t iteration) {
    if (hasStepAt(iteration)) {
      return popStep();
    }
    return 0;
  }
};
