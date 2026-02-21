#include "Game.h"

int main(){

    Game game(1,1); // Temp size, will be overwritten by loadLevel();

    std::vector<std::string> level = {
        "##########",
        "#........#",
        "#..@.....#",
        "#........#",
        "#....#...#",
        "#........#",
        "#........#",
        "##########"
    };

    game.loadLevel(level);
    game.run();

    return 0;
}