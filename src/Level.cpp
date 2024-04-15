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
* Level Objects
* Initiates Level type
* Calls player Object, 
* Calls multiple Enemy objects, depending of Difficulty level
* Keeps track of enemy and player status
* Keeps track of object collisions
******************************************************************************/

#include "Level.h"
#include "Screen.h"
#include "DEBUGF.h"

//Constructor / Destructor
Level::Level(int difficulty, Control *C) : C(C) {
	//this->init(difficulty, C);
	this->levelCompleted = false;
	this->playerAlive = true;
  this->aTimer =0;
  this->initPlayer(C); // init player once, no renewal
  for(int i = 0; i < MAXENEMY*MAXROWS; i++) {this->m_enemyArray[i] = new Enemy( 0,  0,  0, i,0);} // init all enemies once, set to inactive, no renewal
}

Level::~Level() // destructir
{
	delete this->m_player;
  delete this->C;
	for(int i = 0; i < enemyAmount; i++) {delete this->m_enemyArray[i];}
}

//Getters

int Level::enemiesDead()
{
  int d = 0;
	for(int i = 0; i < this->enemyAmount; i++)
		if(!this->m_enemyArray[i]->getAlive()){
				d++;
    }
	return d; // returning number of enemies dead
}

int Level::getScore()
{
	return this->m_player->getScore();
}

int Level::getKills()
{
	return this->m_player->getKills();
}

bool Level::getLevelCompleted()
{
	return this->levelCompleted;
}

bool Level::getPlayerAlive(){
	return this->playerAlive;
}

//Setters
void Level::setDifficulty(int difficulty)
{
	this->m_difficulty = difficulty;
}

//Initializers
void Level::renew(int difficulty, Control *C)
{
  this->levelCompleted = false;
	this->playerAlive = true;
  this->aTimer =0;
	this->setDifficulty(difficulty);
  this->m_player->renew();
  DEBUGF(" * Restart Level to difficulty # %i\n\r",difficulty);
	switch(this->m_difficulty)
	{
			//easy difficulty
		case 1:
			this->initEnemies(MAXENEMY*2,  0); // 2 rows
			break;
			//normal difficulty
		case 2:
			this->initEnemies(MAXENEMY*3, 0); // 3 rows
			break;
			
			//hard difficulty
		case 3:
			this->initEnemies(MAXENEMY*4, 1); // 4 rows
			break;
			
			//harder difficulty
		case 4:
			this->initEnemies(MAXENEMY*3, 3); // 3 rows low
			break;

 			//pro difficulty     
		case 5:
			this->initEnemies(MAXENEMY*4, 3); // 4 rows low
			break;    

  			//nightmare difficulty           
		case 6:
			this->initEnemies(MAXENEMY*3, 5); //3 row very low
			break;  

   			//hell on earth difficulty               
		default:
    	this->initEnemies(MAXENEMY*4, 5); // 4 rows very low
			break;
	}
}


void Level::initPlayer( Control *C)
{
	m_player = new Player( C);
}

void Level::initEnemies(int amount,int Orbit)
{
	this->enemyAmount = amount;
  DEBUGF(" * Activating %i Enemies\n\r",this->enemyAmount);
	for(int i = 0; i < this->enemyAmount; i++){
		if(i < (MAXENEMY) )
			this->m_enemyArray[i]->renew( ENEMYROWX+ i*ENEMYSX*3/2, ENEMYROWY + Orbit*ENEMYSY,  3,i,1);
		else if(i>=MAXENEMY && i<(MAXENEMY*2))
			this->m_enemyArray[i]->renew( ENEMYROWX+ (i-MAXENEMY)*ENEMYSX*3/2 +ENEMYSX/2, ENEMYROWY + 4*ENEMYSY/3 + Orbit*ENEMYSY,  1,i,2);
		else if(i>=(2*MAXENEMY) && i < (3*MAXENEMY))
			this->m_enemyArray[i]->renew( ENEMYROWX+ (i-2*MAXENEMY)*ENEMYSX*3/2, ENEMYROWY + 8*ENEMYSY/3+ Orbit*ENEMYSY,  2,i,8);
		else if(i>=(3*MAXENEMY) && i < (4*MAXENEMY))
			this->m_enemyArray[i]->renew( ENEMYROWX+ (i-3*MAXENEMY)*ENEMYSX*3/2 +ENEMYSX/2, ENEMYROWY + 12*ENEMYSY/3+ Orbit*ENEMYSY,  1,i,12);		
	}
  for(int i = this->enemyAmount; i < MAXENEMY*MAXROWS; i++){ this->m_enemyArray[i]->renew(0,0,0,i,0);} // de-init other enemies
  DEBUGF(" * Activated %i Enemies\n\r",this->enemyAmount);
}

//Functions

void Level::update()
{
	if(!this->levelCompleted || this->playerAlive)
	{
    int ebx,eby,pbx,pby,bh1,bh2;
		this->m_player->update();
    aTimer++ ; 
    bh1=0;bh2=0; // bullit hit to speed up the checks
		for(int i = this->enemyAmount-1; i >=0 ; i--){ // check enemies, start with lower rows first
			//while the enemy is still alive and 2 bullits did not hit
			if(this->m_enemyArray[i]->getAlive() ){
				//check collisions of the player's bullet 
				if(!bh1) if(this->m_enemyArray[i]->collide(this->m_player->getBX(), this->m_player->getBY())){
					bh1=1;
          this->m_player->stopShoot();					                                                      //stop the bullet
					this->m_player->addPoints(m_enemyArray[i]->getType());					                            //and add points to the player, different point per type
          if (m_enemyArray[i]->getAttack()) this->m_player->addPoints(m_enemyArray[i]->getType()*2);  //add extra points if attacker
          this->m_player->addKill();					                                                        //and award kill to the player          
				}
				//check collisions of the player's bullet2 
				if(!bh2) if(this->m_enemyArray[i]->collide(this->m_player->getBX2(), this->m_player->getBY2())){
          bh2=1;
					this->m_player->stopShoot2();					                                                      //stop the bullet2
					this->m_player->addPoints(m_enemyArray[i]->getType());					                            //and add points to the player, different point per type
          if (m_enemyArray[i]->getAttack()) this->m_player->addPoints(m_enemyArray[i]->getType()*2);  //add extra points if attacker
          this->m_player->addKill();					                                                        //and award kill to the player 
				}

				ebx = this->m_enemyArray[i]->getBulletX();
        eby = this->m_enemyArray[i]->getBulletY();
				//if enemy's bomb collides with the player
				if ( eby> (BOMBEND-PLAYERSY-1) )
         if(  this->m_player->collide(ebx, eby,Bomb.width,Bomb.height) )  this->playerAlive = false;
				
				//update the enemy for attack
        if ( (aTimer > ATTACKRATE) && (! this->m_enemyArray[i]->getAttack()) )
            {
            aTimer = 0;
            this->m_enemyArray[i]->setAttack(); // attack
            }
				this->m_enemyArray[i]->update();
			} // enemy is alive
		}
		if (this->enemiesDead()>=this->enemyAmount) this->levelCompleted =true;
	}
	
}

void Level::render()
{

	for(int i = 0; i < this->enemyAmount; i++)
      {
  		if( this->m_enemyArray[i]->getAlive() ) this->m_enemyArray[i]->render();
	    }
  this->m_player->render();
}