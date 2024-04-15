/*****************************************************************************
* | File      	: Space Invaders Demo  / SpaceInvaders.ino
* | Function    :	Demo for Portenta H7  with HAT expansion board , USB-C Video and BLE remote control interface
* | Info        : JV 2024
* | Github      : https://github.com/javos65/Portenta-H7-Space-Invaders
*----------------
* |	This version:   V2.0
* | Date        :   2024-03-16
* | IOriginal   :   Objecrt oriented setup : https://github.com/YXHYX/arduino-space-invaders
*
* Player Definitions
*
******************************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

// Includes
#include "Control.h"
//#include "Screen.h"


namespace{}

class Player
{
private:
	int x,y;
	int prevx,prevy; //used to clear 
	
	int bulletx, bullety;
	int bullet2x, bullet2y;
	int exbulletx, exbullety;
	int exbullet2x, exbullet2y;  
	bool bulletShoot;
  bool bullet2Shoot;
	
	bool alive;
  bool explode;
	int ships;
	int score;
  int kills;
  int animation;
  Control* C;
	

public:
	Player(Control *C);
	virtual ~Player();

  void renew(); // renew function, to avoid constant delete and generate new class -> memory leak!
	int getScore();
  int getKills();
	
	int getBX();
	int getBY();
  int getBX2();
	int getBY2();
	bool collide(int x1, int y1,int sx, int sy);
	
	bool getAlive();
	
	void shoot();
	void stopShoot();
  void stopShoot2();
	void addPoints(int points);
  void addKill();
	
  void update();
  void render();
	
	
};
#endif  //PLAYER_H
