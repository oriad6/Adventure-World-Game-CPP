#include "Results.h"
#include <fstream>
#include <iostream>

Results Results::loadResults(const std::string &filename) {
  Results res;
  std::ifstream file(filename);
  if (!file.is_open())
    return res;

  size_t count;
  file >> count;

  char temp;
  file.get(temp);
  if (temp != '\n') {
    std::string dummy;
    std::getline(file, dummy);
  }

  for (size_t i = 0; i < count; ++i) {
    size_t iter;
    int typeVal;

    file >> iter >> typeVal;

    std::string data;
    std::getline(file, data);

    if (!data.empty() && data[0] == ' ') {
      data.erase(0, 1);
    }
    while (!data.empty() && (data.back() == '\r' || data.back() == ' ')) {
      data.pop_back();
    }

    res.addEvent(iter, static_cast<Results::EventType>(typeVal), data);
  }
  file.close();
  return res;
}

void Results::saveResults(const std::string &filename) const {
  std::ofstream file(filename);
  file << events.size() << "\n";

  for (const auto &e : events) {
    file << e.iteration << " " << static_cast<int>(e.type) << " " << e.data
         << "\n";
  }
  file.close();
}

bool Results::isEqual(const Results &other) const {
  if (events.size() != other.events.size())
    return false;

  auto it1 = events.begin();
  auto it2 = other.events.begin();

  while (it1 != events.end()) {
    if (it1->iteration != it2->iteration || it1->type != it2->type ||
        it1->data != it2->data) {
      return false;
    }
    it1++;
    it2++;
  }
  return true;
}
