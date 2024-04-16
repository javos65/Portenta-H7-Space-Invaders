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
* Level Configs
*
******************************************************************************/

#ifndef LEVEL_H
#define LEVEL_H

#include "Control.h"
#include "Player.h"
#include "Enemy.h"
#include "Bunker.h"


class Level
{
	
private:
	
	int m_difficulty;
	int enemyAmount;
  int bunkerAmount;
  int aTimer;
	
	bool levelCompleted;
	bool playerAlive;
	
	Player* m_player;
	Enemy* m_enemyArray[MAXENEMY*MAXROWS];
  Bunker* m_bunkerArray[MAXBUNKER*BUNKERGRIDX*BUNKERGRIDY];
  Control* C;
	
	//Getters
	int enemiesDead();
	
public:
	Level(int difficulty, Control *C);
	virtual ~Level();
	
	//Getters
	int getScore();
  int getKills();
	bool getLevelCompleted();
	bool getPlayerAlive();
	int getDifficulty();
	//Setters
	void setDifficulty(int difficulty);

	//Initializers
	void renew(int difficulty, Control *C);
	void initPlayer( Control *C);
	void initEnemies(int amount, int Orbit);
  void initBunkers(int amount);
	
	//functions
	void levelComplete();
	void update();
	void render();
	
};

#endif