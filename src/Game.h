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

private:
    void moveEntity(Entity& e, int dx, int dy){
        int newX = e.x + dx;
        int newY = e.y + dy;

        // Clamp to borders
        if(newX < 0) {newX = 0;}
        if(newX >= width) {newX = width-1;}
        if(newY < 0) {newY = 0;}
        if(newY >= height) {newY = height -1;}

        // Check collision with walls
        for(auto& other : entities){
            if(other.symbol == '#' && other.x == newX && other.y == newY) {return;}
        }

        e.x = newX;
        e.y = newY;
    }
 
    // Handle one keyboard input event
    // !! ONLY ENG LAYOUT !!
    void handleInput(int input){
        if(playerIndex < 0 || playerIndex > (int)entities.size()){
            return;
        }

        Entity& player = entities[playerIndex];

        // Convert input to lowercase to ignore CapsLock/Shift
        char c = (char)std::tolower(input);

        if(c == 'w') {moveEntity(player,0,-1);}
        if(c == 's') {moveEntity(player,0,+1);}
        if(c == 'a') {moveEntity(player,-1,0);}
        if(c == 'd') {moveEntity(player,+1,0);}
    }

public:
    Game(int w, int h) : width(w), height(h), playerIndex(-1) {}

    void loadLevel(const std::vector<std::string>&map){
        entities.clear();
        playerIndex = -1;

        height = (int)map.size();
        width = (height > 0) ? (int)map[0].size() : 0;

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                char c = map[y][x];

                if(c == '#'){
                    entities.push_back(Entity(x,y,'#'));
                }
                if(c == '@'){
                    entities.push_back(Entity(x,y,'@'));
                    playerIndex = (int)entities.size() - 1;
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
                    if (e.x == x && e.y == y) cell = e.symbol;
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
            if(c == 'q') {running = false;}
            else {handleInput(input);}
        }
    }
};

#endif // GAME_H