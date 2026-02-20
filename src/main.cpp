#include "Game.h"

int main()
{
    Game game(5, 5);                    // Create a 5x5 grid
    game.addEntity(Entity(2, 2, '@'));  // Add a player entity

    game.render();                      // Display the grid
    return 0;
}