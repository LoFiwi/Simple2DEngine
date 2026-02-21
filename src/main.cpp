#include <iostream>
#include <conio.h>
#include <cstdlib>

#include "Game.h"

void movePlayer(Entity& player, char input, int width, int height){
    // Move up
    if(input == 'w' && player.y > 0){
        player.y--;
    }
    // Move down
    if(input == 's' && player.y < height - 1){
        player.y++;
    }
    // Move left
    if(input == 'a' && player.x > 0){
        player.x--;
    }
    // Move right
    if(input == 'd' && player.x < width - 1){
        player.x++;
    }
}

int main(){
    Game game(10, 10);               
    game.addEntity(Entity(5, 5, '@')); // Add a player entity

    bool running = true;

    while(running){
        system("cls");  // Clear console(Win)

        game.render();

        char input = _getch();  // Get key

        if (input == 'q'){
            running = false;
        }

        movePlayer(game.getEntity(0), input, 10, 10);
    }
                    
    return 0;
}