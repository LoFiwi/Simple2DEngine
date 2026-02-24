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
#include "Tile.h"

// This class manages the game world and rendering
class Game{
private:
    int width, height;              // Grid size
    int playerIndex;                // Index of player entity
    int pendingDx, pendingDy;       // Buffering player movement

    std::vector<std::vector<TileType>> grid;    // Static map tiles, like walls
    std::vector<Entity> entities;   // List of all entities
    
    std::vector<std::vector<int>> entityAt; // Occupance grid: store entities index at [y][x], or -1 if empty

    // Game state
    bool gameOver;  // True when player collided with enemy
    std::vector<std::string> currentLevelMap;   // Stored to allow quick restart

    // Time control for enemy movement(ticks)
    std::chrono::steady_clock::time_point lastEnemyMove;
    int enemyMoveDelayMs;

private:

    // Convert tile type to a character for console
    char tileToChar(TileType t) const{
        if(t == TILE_WALL){
            return '#';
        }
        return '.';
    }

    // Check if coords are inside the grid
    bool inBounds(int x, int y) const{
        return (x >= 0 && x < width && y >= 0 && y < height);
    }

    // Check if a cell is blocked for move
    bool isBlocked(int x, int y) const{
        if(!inBounds(x,y)){
            return true;
        }
        return grid[y][x] == TILE_WALL;
    }

    // Try to move entity by index and update occupancy grid
    bool tryMoveEntity(int entityIndex, int dx, int dy){

        if(entityIndex < 0 || entityIndex >= entities.size()){
            return false;
        }

        if(dx == 0 && dy == 0){
            return false;
        }

        Entity& e = entities[entityIndex];

        int newX = e.x + dx;
        int newY = e.y + dy;

        // Static collision (walls/bounds)
        if(isBlocked(newX, newY)){
            return false;
        }

        // Dynamic collision (who is in target cell?)
        int otherIndex = entityAt[newY][newX];
        if(otherIndex != -1 && otherIndex != entityIndex){
            EntityType a = e.type;
            EntityType b = entities[otherIndex].type;

            bool playerEnemy =
            (a == ENTITY_PLAYER && b == ENTITY_ENEMY) ||
            (a == ENTITY_ENEMY && b == ENTITY_PLAYER);

            if(playerEnemy){
                entityAt[e.y][e.x] = -1;    // Remvoe from old cell
                e.x = newX;
                e.y = newY;
                entityAt[e.y][e.x] = entityIndex;   // Occupy new cell

                gameOver = true;
                return true;
            }
            return false;
        }

        entityAt[e.y][e.x] = -1;    // Free old cell
        e.x = newX;
        e.y = newY;
        entityAt[e.y][e.x] = entityIndex;   // Occupy new cell

        return true; 
    }

    void restartLevel(){

        gameOver = false;
        pendingDx = 0;
        pendingDy = 0;

        // Reload level from stored map
        loadLevel(currentLevelMap);

        // Reset enemy timer to avoid instant movement after restart
        lastEnemyMove = std::chrono::steady_clock::now();
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

        if(gameOver){
            return;
        }

        // Move player
        if(playerIndex >= 0 && playerIndex < (int)entities.size()){
            (void)tryMoveEntity(playerIndex, pendingDx, pendingDy);
            if(gameOver){
                return;
            }
        }

        // Time based enemy movement
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEnemyMove).count();

        if(elapsed >= enemyMoveDelayMs){
            // (patrol left-right)
            for(int i = 0; i < (int)entities.size(); ++i){
                if(entities[i].type == ENTITY_ENEMY){
                    // If enemey has no direction yet, set default direction to left
                    if(entities[i].vx == 0 && entities[i].vy == 0){
                       entities[i].vx = -1;
                       entities[i].vy = 0;
                    }

                    bool moved = tryMoveEntity(i, entities[i].vx, entities[i].vy);
                    if(gameOver){
                        return;
                    }

                    if(!moved){
                        entities[i].vx = -entities[i].vx;
                        entities[i].vy = -entities[i].vy;
                        (void)tryMoveEntity(i, entities[i].vx, entities[i].vy);

                        if(gameOver){
                            return;
                        }
                    }
                }
            }
           lastEnemyMove = now;
        }
    }
        


public:
    Game(int w, int h) 
        : width(w), height(h),
        grid(),
        entities(),
        entityAt(), 
        playerIndex(-1), 
        pendingDx(0), 
        pendingDy(0),
        gameOver(false),
        currentLevelMap(), 
        enemyMoveDelayMs(200) {
            lastEnemyMove = std::chrono::steady_clock::now();
        }

    void loadLevel(const std::vector<std::string>&map){
        // Store map to allow restart
        currentLevelMap = map;

        entities.clear();
        playerIndex = -1;
        gameOver = false;

        height = (int)map.size();
        width = (height > 0) ? (int)map[0].size() : 0;

        grid.assign(height, std::vector<TileType>(width,TILE_EMPTY));
        entityAt.assign(height, std::vector<int>(width,-1));

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){

                char c = map[y][x];

                if(c == '#'){
                    grid[y][x] = TILE_WALL;
                }
                else{
                    grid[y][x] = TILE_EMPTY;
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


        for(int i =0; i < (int)entities.size();++i){
            int ex = entities[i].x;
            int ey = entities[i].y;
            if(inBounds(ex,ey)){
                entityAt[ey][ex] = i;
            }
        }

        // Ensure enemy timer starts "fresh" after loading a level
        lastEnemyMove = std::chrono::steady_clock::now();
    }

    // Render the grid and all entities
    void render(){
        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){

                // Take tileType symbol
                char cell = tileToChar(grid[y][x]);

                // If there any entity, draw it
                int idx = entityAt[y][x];
                if(idx != -1){
                    cell = entities[idx].symbol;
                }

                std::cout << cell;
            }
            std::cout << "\n";  // New line after each row
        }
        if(gameOver){
            std::cout << "==========================" << std::endl;
            std::cout << "==    !GAME    OVER!    ==" << std::endl;
            std::cout << "==========================" << std::endl;
            std::cout << "Press 'R' to restart ro 'Q' to quit." << std::endl;
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
                if(c == 'r'){
                    restartLevel();
                }
                if(!gameOver){
                    handleInput(input);
                }  
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