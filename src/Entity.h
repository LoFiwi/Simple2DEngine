#ifndef ENTITY_H
#define ENTITY_H

class Entity{
public:
	int x,y;		// Position
	char symbol;	// How entity will appear on the screen

	Entity(int xPos, int yPos, char s) : x(xPos), y(yPos), symbol(s){}

};

#endif