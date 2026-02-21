#pragma once

#include "Steps.h"
#include "console.h"
#include <cctype>

class InputController {
public:
  virtual ~InputController() = default;
  virtual char getAction(int timer) = 0;
};

class KeyboardInput : public InputController {
public:
  char getAction(int timer) override {
    if (check_kbhit()) {
      return (char)get_single_char();
    }
    return 0;
  }
};

class FileStepsInput : public InputController {
  Steps &steps;

public:
  FileStepsInput(Steps &s) : steps(s) {}

  char getAction(int timer) override {
    if (steps.hasStepAt(timer)) {
      char c = steps.peekStep(timer);
      if (std::isupper(c)) {
        return 0; // Do not consume riddle answers
      }
      return steps.popStep(); // Consume movement/other keys
    }
    return 0;
  }
};
