#include "Riddle.h" 

using std::cout, std::endl;

Riddle::Riddle(const std::string& q, const std::string opts[NUM_OPTS], char ans) : question(q), correctAnswer(ans) {
	for (int i = 0; i < NUM_OPTS; ++i)
		options[i] = opts[i];
}

void Riddle::display() const {
	cout << question << endl;
	for (int i = 0; i < NUM_OPTS; ++i)
		cout << (char)('A' + i) << ") " << options[i] << endl;
}
