#ifndef ENTITY_H
#define ENTITY_H

// List of entity types
enum EntityType{
	ENTITY_PLAYER,
	ENTITY_WALL,
	ENTITY_ENEMY
};

class Entity{
public:
	int x,y;			// Position
	int vx,vy;			// Velocity
	char symbol;		// How entity will appear on the screen
	EntityType type;	// Entity type 

	Entity(int xPos, int yPos, char s, EntityType t) 
		: x(xPos), y(yPos), vx(0), vy(0), symbol(s), type(t){}

};

#endif