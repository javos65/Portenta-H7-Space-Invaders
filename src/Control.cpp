/*****************************************************************************de
* | File      	: Space Invaders Demo  / SpaceInvaders.ino
* | Function    :	Demo for Portenta H7  with HAT expansion board , USB-C Video and BLE remote control interface
* | Info        : JV 2024
* | Github      : https://github.com/javos65/Portenta-H7-Space-Invaders
*----------------
* |	This version:   V2.0
* | Date        :   2024-03-16
* | IOriginal   :   Objecrtoriented setup : https://github.com/YXHYX/arduino-space-invaders
*
* BLE control object
*
******************************************************************************/
#include "Control.h"
#include "Screen.h"
#include "BLEconfig.h"

Control::Control() 
  {
	this->pressed_x = 0, this->pressed_y = 0, this->pressed_s = 0,this->pressed_l = 0;
	this->prevx = 0, this->prevy = 0, this->prevs = 0;
  this->keyvalue = 0;
  this->BLEconnected = 0;
}

Control::~Control(){  
}

void Control::getKeys(){
	
  uint16_t c=0x0000;
  if(this->BLEconnected){
       c = BLEgetKey();          // read ble scancode 16 bit 0xffxx = pressed, 0x00xx = depressed
          switch(c) {
              case 0xffae : {this->prevx = this->pressed_x;this->pressed_x = 1;break;} // right arrow pressed
              case 0xffac : {this->prevx = this->pressed_x;this->pressed_x = -1;break;} // left arrow pressed    
              case 0xffad : {this->prevy = this->pressed_y;this->pressed_y = 1;break;} // up arrow pressed
              case 0xffaf : {this->prevy = this->pressed_y;this->pressed_y = -1;break;} // dowb arrow pressed   
              case 0xffa3 : {this->prevs = this->pressed_s;this->pressed_s = 1;this->pressed_l = 1;break;} // fire pressed      

              case 0x00ae : {this->prevx = this->pressed_x;this->pressed_x = 0;break;} // right arrow depressed
              case 0x00ac : {this->prevx = this->pressed_x;this->pressed_x = 0;break;} // left arrow depressed    
              case 0x00ad : {this->prevy = this->pressed_y;this->pressed_y = 0;break;} // up arrow depressed
              case 0x00af : {this->prevy = this->pressed_y;this->pressed_y = 0;break;} // dowb arrow depressed   
              case 0x00a3 : {this->prevs = this->pressed_s;this->pressed_s = 0;this->pressed_l = 0;break;} // fire depressed         
              default : {;}
	            }
      }

}
void Control::init(){	 
BLEinit();
}

void Control::clearKeys(){	 
	this->pressed_x = 0; this->pressed_y = 0; this->pressed_s = 0;this->pressed_l = 0;
	this->prevx = 0; this->prevy = 0; this->prevs = 0;
  this->keyvalue = 0;
}

int Control::getCX(){
	return this->pressed_x;
}

int Control::getCY(){
	return this->pressed_y;
}

int Control::getCS(){
	return this->pressed_s;
}

int Control::getLINK(){
	return this->pressed_l;
}

int Control::getBLE(){
  this->BLEconnected = BLECheck();
	return this->BLEconnected;
}
