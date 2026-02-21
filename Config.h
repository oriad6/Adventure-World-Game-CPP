#pragma once
#include <string>
#include <vector>

enum class GameMode { REGULAR, SAVE, LOAD };

class ConfigManager {
public:
	static void parseArgs(int argc, char* argv[], GameMode& mode, bool& isSilent) {
		mode = GameMode::REGULAR;
		isSilent = false;

		if (argc > 1) {
			std::string arg1 = argv[1];
			if (arg1 == "-save")
				mode = GameMode::SAVE;
			else if (arg1 == "-load") {
				mode = GameMode::LOAD;
				if (argc > 2 && std::string(argv[2]) == "-silent") 
					isSilent = true;
			}
		}
	}
};