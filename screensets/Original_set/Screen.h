/*****************************************************************************
* | File      	: Space Invaders Demo  / SpaceInvaders.ino
* | Function    : Demo for Portenta H7  with HAT expansion board , USB-C Video and BLE remote control interface
* | Info        : JV 2024
* | Github      : https://github.com/javos65/Portenta-H7-Space-Invaders
*----------------
* |	This version:   V2.0
* | Date        :   2024-03-16
* | IOriginal   :   Objectoriented setup : https://github.com/YXHYX/arduino-space-invaders
*
* Screen definitions
*
******************************************************************************/
#ifndef SCREEN_H
#define SCREEN_H
#include <Arduino.h>
#include "H7Canvas.h"


//
// SCREEN and GRAPHICS definitions for Game
//

#define AUTORESTART 10000


#define GAMEX 10 // absolute start game screen : relative to sprite screen
#define GAMEY 10  // absolute start geme screen
#define SCREENSX SPRITE_RESX  
#define SCREENSY SPRITE_RESY

#define ENEMYSX 36 // must be equal to sprite size !!
#define ENEMYSY 26 // must be equal to sprite size !!

#define PLAYERSX 36 // must be equal to sprite size !!
#define PLAYERSY 26 // must be equal to sprite size !!

// enemy 
#define BOMBSPEED 8
#define ENEMYROWX (ENEMYSX/3 )       // leave one 1/3 size on the side
#define ENEMYROWY (GAMEY + ENEMYSY/2 )        // leave one 2* size on the side
#define BOMBEND    (SCREENSY - 20)         // the bottom
#define MAXENEMY   (3*SCREENSX)/(ENEMYSX*5)  // Max Enemy per row
#define MAXROWS     4
#define BUNKERLIMIT (BOMBEND - ENEMYSY*4)      // the dow limit for invaders

#define ZIGZAGTICK  29    // ticktime tick between zig zag        ! prime
#define DOWNTICK    401   // tick time tich between lower orbit   ! prime
#define ATTACKRATE  499   // tick time for next attack            ! prime

// Player
#define PLAYERSPEED 6
#define LASERSPEED 12
#define SKYLIMIT  GAMEY              // the SKY
#define PLAYERROWX  (SCREENSX/2)
#define PLAYERROWY  BOMBEND-PLAYERSY+1
#define PLAYERENDX1 (GAMEX-1)
#define PLAYERENDX2 (SCREENSX-PLAYERSX-GAMEX-1)

extern G_image ship;
extern G_image Ray__;
extern G_image Bomb;
extern G_image BombE;
extern G_image Lexplode2;
extern G_image Lexplode;
extern G_image Laser ;
extern G_image Laser2 ;
extern G_image Invaders4_2;
extern G_image Invaders4_1;
extern G_image Invaders3_2;
extern G_image Invaders3_1;
extern G_image Invaders2_2;
extern G_image Invaders2_1;
extern G_image Invaders1_2;
extern G_image Invaders1_1;
extern G_image Iexplode2;
extern G_image Iexplode;

extern G_image LevelUp;
extern G_image LevelOver;
extern G_image GameOver;
extern G_image Splash ;
extern G_image BLE_on;
extern G_image BLE_off;
extern G_image Arduino ;
extern G_image STMicro2;
extern G_image STMicro;
extern G_image SpaceInvaders;


#endif  //SCREEN_H