
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
* Bunker Objects
* Sense Collision with laser bullets
******************************************************************************/
#include "src/H7Canvas.h"      // Graph functions for H7 USB-C Video
#include "src/DEBUGF.h"
#include "Bunker.h"
#include "Screen.h"

//constructor 
Bunker::Bunker(int x, int y, int Ti, int ID)  {
	m_x = x, m_y 	= y; 	//intialize position
	alive 			= true;	//intialize life state - callable by Level()
  type        = Ti;   // type of bunkerpiece 1 / 2  = blocks,  3 / 4  = edge , / 0 = not active 
  if (type == 0) {alive=false, m_x=0;m_y=0;}  // dont build block type 0
  m_ID = ID;
  animation =0;
}
//deconstructor
Bunker::~Bunker(){

}

void Bunker::renew(int xx, int yy, int Tii,int IDD)
{
	m_x = xx, m_y 	= yy; 	                    //intialize position
	alive = true;	                              //intialize life state - callable by Level()
  type = Tii;                                 // type of bunkerpiece 1 / 2  = blocks,  3 / 4  = edge , / 0 = not active
  if (type == 0) {alive=false, m_x=0;m_y=0;}  // dont build block type 0
  m_ID = IDD;
  animation =0;
}

//check collisions with laser
bool Bunker::collide(int x1, int y1, int bx,int by){
	
	if( (x1)    < (m_x + BUNKERSX-1)  &&
		  (x1+bx) > (m_x+1)             &&
		  y1      < (m_y + BUNKERSY)  &&
		  (y1+by) > (m_y )  )
	{
		//collision detected : Laser vs Bunker or Bomb vs Bunker
    this-> animation++;
    if (this->animation >= 2) {this->alive=false; this->type=0;}
    return true;
	  } 
  else return false;  

}


void Bunker::update(){ } // no updates, static object

void Bunker::render(){

if(this->alive == true) { // only render when alive,
			  if(animation == 0 ){
         switch(this->type) {
				    case 1 : Bunk1.xpos= m_x;Bunk1.ypos=m_y;Sprite_DrawImage(Bunk1);   ;break;
				    case 2 : Bunk2.xpos= m_x;Bunk2.ypos=m_y;Sprite_DrawImage(Bunk2);   ;break; 
				    case 3 : Bunk3.xpos= m_x;Bunk3.ypos=m_y;Sprite_DrawImage(Bunk3);   ;break; 
            case 4 : Bunk4.xpos= m_x;Bunk4.ypos=m_y;Sprite_DrawImage(Bunk4);   ;break; 
           }               
        }
			  if(animation==1 ){
          switch(this->type) {
				    case 1 : Bunk1_x.xpos= m_x;Bunk1_x.ypos=m_y;Sprite_DrawImage(Bunk1_x);   ;break;
				    case 2 : Bunk2_x.xpos= m_x;Bunk2_x.ypos=m_y;Sprite_DrawImage(Bunk2_x);   ;break; 
				    case 3 : Bunk3_x.xpos= m_x;Bunk3_x.ypos=m_y;Sprite_DrawImage(Bunk3_x);   ;break; 
            case 4 : Bunk4_x.xpos= m_x;Bunk4_x.ypos=m_y;Sprite_DrawImage(Bunk4_x);   ;break; 
            }  
			  }
  }
}

bool Bunker::getAlive(){
	return this->alive;
}

int Bunker::getType(){
	return this->type;
}

int Bunker::getX(){
	return this->m_x;
}

int Bunker::getY(){
	return this->m_y;
}
