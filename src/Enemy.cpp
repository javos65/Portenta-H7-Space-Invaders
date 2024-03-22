
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
* Enemy Objects
* Controls automatic movement and animation
* Controls bombing 
* Sense Collision with laser bullets
******************************************************************************/
#include "Enemy.h"
#include "Screen.h"
#include "USBC_Canvas.h"      // Graph functions for H7 USB-C Video
#include "SDebug.h"

//constructor 
Enemy::Enemy(int x, int y, int Ti, int ID)  {
	m_x = x, m_y 	= y; 	//intialize position
  m_prevx=x, m_prevy=y;
	m_bombx = x, m_bomby = y;
	animationFrame 	= 0; 	//intialize frame of animation
	alive 			= true;	//intialize life state - callable by Level()
  twilight    = false; // twilight stage, when hit, but still exploding or falling bombs
  exploded    = false;
  explodingA    = 0;
	timerb 			= random(550, 1500);
  timerx 			= 0;
  timery 			= 0;
  type        = Ti; // type of invader 1 / 2 / 3 / 0= not active
  shift       = 0; // shift
  sidewinder  = 0 ;
  bombrate    = 1;
  bombexplode = 0;
  dropBomb    = false;
  attack      = false;
  xdirection  = 1;
  m_ID = ID;
}
//deconstructor
Enemy::~Enemy(){

}

void Enemy::renew(int xx, int yy, int Tii,int IDD)
{
	m_x = xx, m_y 	= yy; 	//intialize position
  m_prevx=xx, m_prevy=yy;
	m_bombx = xx, m_bomby = yy;
	animationFrame 	= 0; 	//intialize frame of animation
	alive 			= true;	//intialize life state - callable by Level()
  twilight    = false; // twilight stage, when hit, but still exploding or falling bombs
  exploded    = false;
  explodingA    = 0;
	timerb 			= random(550, 1500);
  timerx 			= 0;
  timery 			= 0;
  type        = Tii; // type of invader 1 / 2 / 3 / 0 = not active
  shift       = 0; // shift
  sidewinder  = 0 ;
  bombrate    = 1;
  bombexplode = 0;
  dropBomb    = false;
  attack      = false;
  xdirection  = 1;
  m_ID = IDD;
}
//check collisions with laser
bool Enemy::collide(int x1, int y1){
  if(this->alive == true && this->twilight == false) { // only collide when alive, no collide in twilight
	if(x1 > m_x-1             &&
		 x1 < (m_x+ENEMYSX+1)   &&
		y1 > m_y-4              &&
		y1 <  (m_y+ENEMYSY+1)   ){
		//collision detected : Laser vs Enemy
    this-> explodingA = 1;     // mark start of explode animation
    this-> twilight = true; // mark enemy lives in twilight: no more new bombing, finish active bomb
    return true;
	  } 
  }

	return false;
}

void Enemy::bomb(bool now, int speed)
{
  if (this->alive == true && this->twilight == false) {    // start drop bombs only when alive and not in twilight
	  this->timerb += bombrate + this->type; if (now == true) this->timerb += 2000; 
	  if( (this->timerb > 2000) && (dropBomb == false) ){	//new bomb only when timer is up and no other bomb was falling
		  this->m_bombx = this->m_x + random(3, ENEMYSX-3);   ;
		  this->m_bomby = this->m_y+ENEMYSY;    
      switch(this->type) {    
        case 1 :  sidewinder = random(-1,1 ); break;
        case 2 :  sidewinder = random(-2,2 ); break; // side winder bombs : moving also sidewards
        case 3 :  sidewinder = random(-3,3 ); break; // side winder bombs : moving also sidewards
        case 4 :  sidewinder = random(-4,4 ); break; // side winder bombs : moving also sidewards
        default : sidewinder = 0; break;
      }
		  this->dropBomb = true;this->bombexplode=0;
		  timerb = random(0,  1000);
      Debug("Enemy ");Debug(m_ID);Debug("/");Debug(type);Debugln(" starts bombing");
	  }
  }	

	if(dropBomb == true ) // track fallen bomb and draw bomb
	{
    Canvas_FillRect(m_bombx, m_bomby, BOMBX, BOMBY, BLACK); // remove old Bomb
    // FALLING BOMB
		if( m_bomby < BOMBEND-BOMBY ){
        m_bomby += BOMBSPEED + speed ;                                      // update bomb positionsY
        if (m_bomby >= BOMBEND-BOMBY) {m_bomby = BOMBEND-BOMBY+1;bombexplode=1;}       // check limit, initiate explode at ground level
        m_bombx += sidewinder;                                              // update bomb positionsX
        if (m_bombx < GAMEX+2 ||m_bombx > GAMEX+SCREENSX-2 ) sidewinder = -sidewinder;    // check side boundaries   
        Canvas_DrawColors((int16_t)m_bombx,(int16_t)m_bomby,(int16_t) BOMBX, (int16_t) BOMBY,(uint16_t *) Bomb); // draw normal bombs
        } // falling

    // EXPLODING BOMB
     if ( bombexplode>0  ) {
          if ( bombexplode >2 ) { // end bomb
                m_bombx = 0, m_bomby = 0;bombexplode=0;
			          dropBomb = false; 
                Debug(m_ID);Debug("/");Debug(type);Debugln(" bomb ended");
              }    
          else  {
              bombexplode++; 
              Canvas_DrawColors((int16_t)m_bombx,(int16_t)m_bomby,(int16_t) BOMBX, (int16_t) BOMBY,(uint16_t *) BombE); // crashed bomb
              Debug(m_ID);Debug("/");Debug(type);Debugln(" bomb exploding");
              }
          }     // exploding  
  }        

}

void Enemy::update(){
	randomSeed(m_x*(analogRead(0)/m_y) ); // generate random seeds
	if(animationFrame == 5)	animationFrame = -1;
	else animationFrame++;
	
  if ( (this->twilight == true) && (this->dropBomb == false) && (this->exploded==true) ) this->alive = false; // mark finaly DEATH !! : no more rendering, no more collision, no more bombing
  else if ( !this->attack) {// normal behaviour
    bomb(false,2);
    this->timerx++; // timer for X-shift of enemy
    this->timery++; // timer for Y-shift of enemy
    if(this->timerx > ZIGZAGTICK){	
          this->timerx =0;
          this->m_prevy= this->m_y;this->m_prevx= this->m_x;;
          switch(shift) {                 // ZigZag in 4 steps
            case 0 : {this->m_x+=ENEMYSX/2;shift=1;break;}
            case 1 : {this->m_x+=ENEMYSX/2;shift=2;break;}
            case 2 : {this->m_x-=ENEMYSX/2;shift=3;break;}
            case 3 : {this->m_x-=ENEMYSX/2;shift=0;break;}
          default : {shift=0;break;}
          }
      }
      else if( (this->timery > DOWNTICK ) && (m_y < BUNKERLIMIT) ){	// once bunker limit is reached, no more down
          this->timery =0;
          this->m_prevy= this->m_y;this->m_prevx= this->m_x;;
          this->m_y+=ENEMYSY;
            this->bombrate += 4; // increase bomb rate
            }  
    }           // end if not attacking
  else {        // attacking : bomb always, increase bomb speed
        bomb(true, 5);
        this->m_prevy= this->m_y;this->m_prevx= this->m_x;;
        if ( (this->m_y < BOMBEND-3*ENEMYSY) ) {  this->m_y+=2*BOMBSPEED/3; } // mov down
        if ( (this->m_x <= GAMEX)  ) {this->m_x = GAMEX ; xdirection = -xdirection;}  
        if ( (this->m_x > (GAMEX+SCREENSX-ENEMYSX)) ) {this->m_x=GAMEX+SCREENSX-ENEMYSX ;xdirection = -xdirection;}
        this->m_x+= (2*BOMBSPEED*xdirection); 
      }       // end attacking    
}

void Enemy::render(){

  if( (m_prevx != m_x)  || (m_prevy != m_y)){     // Clear old enemy, on old place if there was a change
      Canvas_FillRect(m_prevx, m_prevy,ENEMYSX,ENEMYSY, BLACK);
      this->m_prevy= this->m_y;this->m_prevx= this->m_x;;
      }

if (explodingA>0 ) { 
      if (this->explodingA > 4) {
        Canvas_FillRect(m_x, m_y,ENEMYSX,ENEMYSY, BLACK);
        this->exploded = true; // final explosion animation
        this->explodingA++; 
      }
      else {
      Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) Iexplode); 
      this->explodingA++; 
      }
  }
else if(this->alive == true && this->twilight == false) { // only render when alive, but not in twilight
			  if(animationFrame < 3 && animationFrame >= 0){
         switch(this->type) {
				    case 1 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader1_1); ;break;
				    case 2 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader2_1); ;break;
				    case 3 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader3_1); ;break;
            case 4 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader4_1); ;break;
            default : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader2_1); ;break;   
           }               
        }
			  if(animationFrame < 6 && animationFrame >= 3){
          switch(this->type) {
				    case 1 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader1_2); ;break;
				    case 2 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader2_2); ;break;
				    case 3 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader3_2); ;break;
				    case 4 : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader4_2); ;break;            
            default : Canvas_DrawColors((int16_t)m_x,(int16_t)m_y,(int16_t) ENEMYSX, (int16_t) ENEMYSY,(uint16_t *) invader1_2); ;break;
            }  
			  }
     }
}

bool Enemy::getAlive(){
	return this->alive;
}

int Enemy::getType(){
	return this->type;
}

int Enemy::getX(){
	return this->m_x;
}

int Enemy::getY(){
	return this->m_y;
}

int Enemy::getBulletX(){
	return this->m_bombx;
}

int Enemy::getBulletY(){
	return this->m_bomby;
}

void Enemy::setAttack(){
	this->attack = true;
  this->type =4 ; // red attacker
}

bool Enemy::getAttack(){
	return(this->attack);
}
 