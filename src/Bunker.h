
/*****************************************************************************
* | File      	: Space Invaders Demo  / SpaceInvaders.ino
* | Function    :	Demo for Portenta H7  with HAT expansion board , USB-C Video and BLE remote control interface
* | Info        : JV 2024
* | Github      : https://github.com/javos65/Portenta-H7-Space-Invaders
*----------------
* |	This version:   V2.0
* | Date        :   2024-03-16
* | IOriginal   :   Objecrtoriented setup : https://github.com/YXHYX/arduino-space-invaders
*
* Bunker deifnitions
*
******************************************************************************/

#ifndef BUNKER_H
#define BUNKER_H

// Includes
#include "Screen.h"


// Defines

namespace{ }

// Bunker class
class Bunker
{
private:
	int m_x, m_y; //position of Bunker
	int type;
	bool alive;       // life state of Bunker -> alive stops wafter  Bunker till exploded and bombs ended
  int m_ID;
	int animation;

public:
	// Constructor // Destructor
	Bunker( int x, int y,int Ti,int ID);
	virtual ~Bunker();
	
	//Getters
	bool getAlive();
	int getX();
	int getY();
	int getType();


 // Checkers
	bool collide(int x1, int y1, int bx,int by);
		
	//functions
	void update();
	void render();
  void renew(int xx, int yy, int Tii, int IDD); // renew function, to avoid constant delete and generate new class -> memory leak!
};

#endif // !BUNKER