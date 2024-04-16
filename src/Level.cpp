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
#include "src/DEBUGF.h"

//Constructor / Destructor
Level::Level(int difficulty, Control *C) : C(C) {
	//this->init(difficulty, C);
	this->levelCompleted = false;
	this->playerAlive = true;
  this->aTimer =0;
  this->initPlayer(C); // init player once, no renewal
  for(int i = 0; i < MAXENEMY*MAXROWS; i++) {this->m_enemyArray[i] = new Enemy( 0,  0,  0, i,0);} // init all enemies once, set to inactive, no renewal
  for(int i = 0; i < MAXBUNKER*BUNKERGRIDX*BUNKERGRIDY; i++) {this->m_bunkerArray[i] = new Bunker( 0,  0,  0, i);} // init all bunkersonce, set to inactive, no renewal
  DEBUGF(" * Level setup, 1 player, %i enemies, %i bunkerblocks\n\r",MAXENEMY*MAXROWS,MAXBUNKER*BUNKERGRIDX*BUNKERGRIDY);
}

Level::~Level() // destructir
{
	delete this->m_player;
  delete this->C;
	for(int i = 0; i < enemyAmount; i++) {delete this->m_enemyArray[i];}
  for(int t = 0; t < bunkerAmount; t++) {delete this->m_bunkerArray[t];}
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
  this->initBunkers(MAXBUNKER);
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


void Level::initBunkers(int amount)
{
  int index,startx,starty, blueprint[BUNKERGRIDX*BUNKERGRIDY] ={4,1,2,3, 1,2,1,2, 2,0,0,1};
	this->bunkerAmount = amount*BUNKERGRIDX*BUNKERGRIDY;
  DEBUGF(" * Activating %i Bunkers, total %i BunkerBlocks\n\r",amount, this->bunkerAmount);

	for(int i = 0; i < amount; i++){
    for(int u = 0 ; u< BUNKERGRIDY ; u++){
          starty = BUNKERSTART+u*BUNKERSY;
          for(int v = 0 ; v< BUNKERGRIDX; v++ ){
            startx = ( (i*2+1)*Sprite_width()/(amount*2)) - 2*BUNKERSX + v*BUNKERSX;
            index = i*BUNKERGRIDX*BUNKERGRIDY + u*BUNKERGRIDX + v;
            this->m_bunkerArray[index]->renew(startx, starty,blueprint[u*BUNKERGRIDX + v],index); // edge
            }
          }   // u= layers
    } // i = bunkers

  for(int i = this->bunkerAmount; i < MAXBUNKER*BUNKERGRIDX*BUNKERGRIDY; i++){ this->m_bunkerArray[i]->renew(0,0,0,i);} // de-init other enemies
  DEBUGF(" * Activated %i Bunkersblocks\n\r",this->bunkerAmount);
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
				if(!bh1) if(this->m_enemyArray[i]->collide(this->m_player->getBX()-2, this->m_player->getBY())){
					bh1=1;
          this->m_player->stopShoot();					                                                      //stop the bullet
					this->m_player->addPoints(m_enemyArray[i]->getType());					                            //and add points to the player, different point per type
          if (m_enemyArray[i]->getAttack()) this->m_player->addPoints(m_enemyArray[i]->getType()*2);  //add extra points if attacker
          this->m_player->addKill();					                                                        //and award kill to the player          
				}
				//check collisions of the player's bullet2 
				if(!bh2) if(this->m_enemyArray[i]->collide(this->m_player->getBX2()-2, this->m_player->getBY2())){
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
         if(  this->m_player->collide(ebx, eby,Bomb.width-2,Bomb.height) )  this->playerAlive = false;
				   
        //if enemy's bomb collides with the bunker
        for(int u=0; u<this->bunkerAmount ; ++u )
          if ( (eby>= BUNKERSTART) && this->m_bunkerArray[u]->getAlive() ) {
            if (this->m_bunkerArray[u]->collide(ebx, eby,Bomb.width-2,Bomb.height)) {this->m_enemyArray[i]->killBomb();u=bunkerAmount;}
            }

				//update the enemy for attack
        if ( (aTimer > ATTACKRATE) && (! this->m_enemyArray[i]->getAttack()) )
            {
            aTimer = 0;
            this->m_enemyArray[i]->setAttack(); // attack
            }
				this->m_enemyArray[i]->update();
			} // enemy is alive
		}

        //if Laser1 collides with the bunker
				ebx = this->m_player->getBX();
        eby = this->m_player->getBY();
        for(int u=0; u<this->bunkerAmount ; ++u )
          if ( (eby<= BUNKERSTART) &&  this->m_bunkerArray[u]->getAlive() ) {
             if (this->m_bunkerArray[u]->collide(ebx, eby,Ray__.width,Ray__.height)) {this->m_player->stopShoot();u=bunkerAmount;}
             }
        //if Laser2 collides with the bunker
				ebx = this->m_player->getBX2();
        eby = this->m_player->getBY2();
        for(int u=0; u<this->bunkerAmount ; ++u )
          if ( (eby>= BUNKERSTART) && this->m_bunkerArray[u]->getAlive() ) {
            if (this->m_bunkerArray[u]->collide(ebx, eby,Ray__.width,Ray__.height)) {this->m_player->stopShoot2();u=bunkerAmount;}
            }


		if (this->enemiesDead()>=this->enemyAmount) this->levelCompleted =true;
	}
	
}

void Level::render()
{
int i;
	for( i = 0; i < this->bunkerAmount; i++)
      {
  		if( this->m_bunkerArray[i]->getAlive() ) this->m_bunkerArray[i]->render();
	    }
	for( i = 0; i < this->enemyAmount; i++)
      {
  		if( this->m_enemyArray[i]->getAlive() ) this->m_enemyArray[i]->render();
	    }
  this->m_player->render();
}