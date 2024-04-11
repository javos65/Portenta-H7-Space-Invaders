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
* Player object
* Initiates Player Laser
* Movement control of Player Laser
* Keeps track of shoot bullets
* Sense collision with Enemy Bombs
******************************************************************************/

#include "Player.h"
#include "Screen.h"
#include "H7Canvas.h"      // Graph functions for H7 USB-C Video


Player::Player( Control *C) : C(C) {
	this->x = PLAYERROWX , this->y = PLAYERROWY;
	this->prevx = PLAYERROWX ,this->prevy=PLAYERROWY;
	this->bulletx = 0, this->bullety = 0;
  this->exbulletx = 0, this->exbullety = 0;
	this->bulletShoot = false;

  this->bullet2x = 0, this->bullet2y = 0;
  this->exbullet2x = 0, this->exbullet2y = 0;  
	this->bullet2Shoot = false;
	this->prevx = 0, this->prevy = 0;
	this->score = 0;
  this->kills = 0;
	this->explode = false;
	this->alive = true;
 }

Player::~Player(){
    delete this->C;
    this->score = 0;
    this->kills = 0;
}

void Player :: renew()
{
	this->x = PLAYERROWX , this->y = PLAYERROWY;
	this->prevx = PLAYERROWX ,this->prevy=PLAYERROWY;
	this->bulletx = 0, this->bullety = 0;
  this->exbulletx = 0, this->exbullety = 0;
	this->bulletShoot = false;

  this->bullet2x = 0, this->bullet2y = 0;
  this->exbullet2x = 0, this->exbullet2y = 0;  
	this->bullet2Shoot = false;
	this->prevx = 0, this->prevy = 0;
	this->score = 0;
  this->kills = 0;
	this->explode = false;
	this->alive = true;
}
void Player::shoot(){
	
	if(this->C->getCS()==1){
		if(bulletShoot == false){
			this->bulletx = this->x+PLAYERSX/2;
			this->bullety = this->y;
      exbulletx = bulletx; exbullety=bullety;
			this->bulletShoot = true;
		  }
		else if(bullet2Shoot == false && bullety<(Sprite_height()/2) ){ // second shot ! - yeah - only when first shot is half way
			this->bullet2x = this->x+PLAYERSX/2;
			this->bullet2y = this->y;
      exbullet2x = bullet2x; exbullet2y=bullet2y;
			this->bullet2Shoot = true;
		  }      
	  }

	if(bulletShoot == true){ // first bullet
		exbulletx = bulletx; exbullety=bullety; bullety-=LASERSPEED;		
    if(bullety < SKYLIMIT) bulletShoot = false;
	}

	if(bullet2Shoot == true){ // secondbullet
		exbullet2x = bullet2x; exbullet2y=bullet2y; bullet2y -= LASERSPEED;	
    if(bullet2y < SKYLIMIT) bullet2Shoot = false;
	}

}



void Player::stopShoot()
{
  this->bulletx=this->x;this->bullety=this->y;
	bulletShoot = false;
}

void Player::stopShoot2()
{
	this->bullet2x=this->x;this->bullet2y=this->y;
  bullet2Shoot = false;
}

int Player::getScore(){
	return this->score;
}

int Player::getKills(){
	return this->kills;
}

void Player::addPoints(int points){
	this->score = this->score + points;
}	

void Player::addKill(){
	this->kills++; 
}	

bool Player::collide(int x1, int y1, int bx,int by){
	
	if( (x1)    < (x + PLAYERSX-1)  &&
		  (x1+bx) > (x+1)             &&
		  y1      < (y + PLAYERSY-1)  &&
		  (y1+by) > (y +2*PLAYERSY/3)  )
	{
     this->x--;
    for(int i = 0; i<12; ++i)  // make & shake explosion :)
        {
        Sprite_FillRect( this->x, this->y, Lexplode.width, Lexplode.height, RED,ERASEMASK); // clear ship area
        if(i%2) { x = x+3; Lexplode.xpos=this->x;Lexplode.ypos=this->y;Sprite_DrawImage(Lexplode);} // EXPLODE if HIT}   
        else    { x = x-3; Lexplode2.xpos=this->x;Lexplode2.ypos=this->y;Sprite_DrawImage(Lexplode2);} // EXPLODE if HIT }  
        CanvasAll_DrawFrames(true);
        delay(50); // wait after collide
        
        }
    this->alive = false;
    return true;
	}
	
	return false;
}

void Player::update(){
  this->C->getKeys(); // update controller keys
  this->shoot();
	this->prevx = x;
	this->prevy = y;
	if(this->C->getCX()==1)	x += 4;
	else if(this->C->getCX()==-1)	x -= 4;

	//if(this->C->getCY()==-1) y += 4;
	//else if(this->C->getCY()==1) y -= 4;

	if(x > PLAYERENDX2)	x = PLAYERENDX2;
	else if (x < PLAYERENDX1)	x = PLAYERENDX1;
	
	if(y > PLAYERROWY) y = PLAYERROWY;
	else if (y < (PLAYERROWY-2*PLAYERSY))	y = (PLAYERROWY-2*PLAYERSY);
	this->shoot();

}

void Player::render(){
 if (this->alive == true) {               // only render animation if alive, if not alive, explosion is already rendered
    Laser.xpos=x;Laser.ypos=y;Sprite_DrawImage(Laser);
    if(bullet2Shoot == true) {Ray__.xpos=bullet2x;Ray__.ypos=bullet2y;Sprite_DrawImage(Ray__);}
    if(bulletShoot == true) {Ray__.xpos=bulletx;Ray__.ypos=bullety;Sprite_DrawImage(Ray__);}
 }
 
}

int Player::getBX(){
	return this->bulletx;
}

int Player::getBY(){
	return this->bullety;
}

int Player::getBX2(){
	return this->bullet2x;
}

int Player::getBY2(){
	return this->bullet2y;
}

bool Player::getAlive(){
	return this->alive;
}
