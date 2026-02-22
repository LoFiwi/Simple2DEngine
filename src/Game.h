#ifndef GAME_H
#define GAME_H

#include <vector>
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <cctype>
#include <string>

#include "Entity.h"

// This class manages the game world and rendering
class Game{
private:
    int width, height;              // Grid size
    int playerIndex;                // Index of player entity
    std::vector<Entity> entities;   // List of all entities
    int pendingDx, pendingDy;       // Buffering player movement

private:

    bool isWallAt(int x, int y){
        for(auto& e : entities){
            if(e.type == ENTITY_WALL && e.x == x && e.y == y){
                return true;
            }
        }
        return false;
    }

    void moveEntity(Entity& e, int dx, int dy){
        int newX = e.x + dx;
        int newY = e.y + dy;

        // Clamp to borders
        if(newX < 0) {newX = 0;}
        if(newX >= width) {newX = width-1;}
        if(newY < 0) {newY = 0;}
        if(newY >= height) {newY = height -1;}

        if(isWallAt(newX, newY)){
            return;
        }

        e.x = newX;
        e.y = newY;
    }
 
    // Convert input into buffered movement
    // !! ONLY ENG LAYOUT !!
    void handleInput(int input){

        // Reset buffered movement each frame
        pendingDx = 0;
        pendingDy = 0;

        // Convert input to lowercase to ignore CapsLock/Shift
        char c = (char)std::tolower(input);

        if(c == 'w') {pendingDy = - 1;}
        if(c == 's') {pendingDy = 1;}
        if(c == 'a') {pendingDx = -1;}
        if(c == 'd') {pendingDx = 1;}
    }

    void update(){
        if(playerIndex >= 0 && playerIndex < (int)entities.size()){
            Entity& player = entities[playerIndex];
            moveEntity(player, pendingDx, pendingDy);
        }

        // Move enemies automatically. Now it simple movement: enemy tries to go right each update.
        for(auto&e : entities){
            if(e.type == ENTITY_ENEMY){
                moveEntity(e, 1, 0);
            }
        }
    }

public:
    Game(int w, int h) : width(w), height(h), playerIndex(-1), pendingDx(0), pendingDy(0) {}

    void loadLevel(const std::vector<std::string>&map){
        entities.clear();
        playerIndex = -1;

        height = (int)map.size();
        width = (height > 0) ? (int)map[0].size() : 0;

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                char c = map[y][x];

                if(c == '#'){
                    entities.push_back(Entity(x,y,'#', ENTITY_WALL));
                }
                if(c == '@'){
                    entities.push_back(Entity(x,y,'@', ENTITY_PLAYER));
                    playerIndex = (int)entities.size() - 1;
                }
                if(c == 'E'){
                    entities.push_back(Entity(x,y,'E', ENTITY_ENEMY));
                }
            }
        }
    }

    // Render the grid and all entities
    void render(){
        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){
                char cell = '.';   // Default empty cell

                // Check if an entity is in this position
                for (auto& e : entities){
                    if (e.x == x && e.y == y) {
                        cell = e.symbol;
                        break;
                    }
                }

                std::cout << cell;
            }
            std::cout << "\n";  // New line after each row
        }
    }

    // Main game loop
    void run(){
        bool running = true;

        while(running){
            system("cls");  //Clear console(Win)
            render();

            int input = _getch();

            char c = (char)std::tolower(input);
            if(c == 'q') {
                running = false;
                continue;
            }
            handleInput(input);
            update();
        }
    }
};

#endif // GAME_H