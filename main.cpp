#include "GameManager.h"
#include "Config.h"
#include "console.h"

int main(int argc, char* argv[]) {
    init_console();
    hideCursor();

    GameManager game;
    game.initialize(argc, argv);
    game.runMenu();

    return 0;
}