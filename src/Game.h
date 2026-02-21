#ifndef GAME_H
#define GAME_H

#include <vector>
#include <iostream>
#include "Entity.h"

// This class manages the game world and rendering
class Game{
private:
    int width, height;             // Grid size
    std::vector<Entity> entities;  // List of all entities

public:
    Game(int w, int h) : width(w), height(h) {}

    // Add an entity to the game
    void addEntity(const Entity& e) { entities.push_back(e); }

    // Get reference to entity by index
    Entity& getEntity(int index){
        return entities[index];
    }

    // Render the grid and all entities
    void render(){
        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){
                char c = '.';   // Default empty cell

                // Check if an entity is in this position
                for (auto& e : entities){
                    if (e.x == x && e.y == y) c = e.symbol;
                }

                std::cout << c;
            }
            std::cout << "\n";  // New line after each row
        }
    }
};

#endif // GAME_H