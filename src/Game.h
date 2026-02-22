#ifndef GAME_H
#define GAME_H

#include <vector>
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <cctype>
#include <string>
#include <chrono>
#include <thread>

#include "Entity.h"

// This class manages the game world and rendering
class Game{
private:
    int width, height;              // Grid size
    int playerIndex;                // Index of player entity
    int pendingDx, pendingDy;       // Buffering player movement

    std::vector<Entity> entities;   // List of all entities
    
    // Time control for enemy movement(ticks)
    std::chrono::steady_clock::time_point lastEnemyMove;
    int enemyMoveDelayMs;

private:

    bool isWallAt(int x, int y){
        for(auto& e : entities){
            if(e.type == ENTITY_WALL && e.x == x && e.y == y){
                return true;
            }
        }
        return false;
    }

    bool tryMoveEntity(Entity& e, int dx, int dy){
        int newX = e.x + dx;
        int newY = e.y + dy;

        // Clamp to borders
        if(newX < 0) {newX = 0;}
        if(newX >= width) {newX = width-1;}
        if(newY < 0) {newY = 0;}
        if(newY >= height) {newY = height -1;}

        if(isWallAt(newX, newY)){
            return false;
        }

        // If clamping prevented movement, treat as blocked
        if(newX == e.x && newY == e.y){
            return false;
        }

        e.x = newX;
        e.y = newY;
        return true;
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

    // Update world state(player, enemies, etc.)
    void update(){

        // Move player
        if(playerIndex >= 0 && playerIndex < (int)entities.size()){
            Entity& player = entities[playerIndex];
            (void)tryMoveEntity(player, pendingDx, pendingDy);
        }

        // Time based enemy movement
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEnemyMove).count();

        if(elapsed >= enemyMoveDelayMs){
            // (patrol left-right)
            for(auto&e : entities){
                if(e.type == ENTITY_ENEMY){
                    // If enemey has no direction yet, set default direction to left
                    if(e.vx == 0 && e.vy ==0){
                       e.vx = -1;
                       e.vy = 0;
                    }

                    bool moved = tryMoveEntity(e, e.vx, e.vy);

                    if(!moved){
                        e.vx = -e.vx;
                        e.vy = -e.vy;
                        (void)tryMoveEntity(e,e.vx,e.vy);
                    }
                }
            }
           lastEnemyMove = now; 
        }
    }
        


public:
    Game(int w, int h) 
        : width(w), height(h), playerIndex(-1), pendingDx(0), pendingDy(0), enemyMoveDelayMs(200) {
            lastEnemyMove = std::chrono::steady_clock::now();
        }

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
                    Entity enemy(x,y,'E', ENTITY_ENEMY);
                    enemy.vx = -1;
                    enemy.vy = 0;
                    entities.push_back(enemy);
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

            pendingDx = 0;
            pendingDy = 0;

            if(_kbhit()){
                int input = _getch();
                char c = (char)std::tolower(input);

                if(c == 'q') {
                    running = false;
                    continue;
                }
                handleInput(input);
            }

            update();
            system("cls");  //Clear console(Win)
            render();

            // Simple frame limiter (~60fps)
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
};

#endif // GAME_H