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
* Screen definitions
*
******************************************************************************/
#ifndef SCREEN_H
#define SCREEN_H
#include <Arduino.h>
#include "USBC_Canvas.h"


//
// SCREEN and GRAPHICS definitions for Game
//
#define CLEAR     0x0005
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define LIME	    0x2EFF
#define CYAN      0x07FF
#define DARK_CYAN 0x0DCB
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF
#define ORANGE    0xfd20

#define LCDWIDTH  GENERIC_RESX  // portrait mode 800 x 600 USB-C Video
#define LCDHEIGHT GENERIC_RESY 

#define AUTORESTART 10000

#define GAMEX 200 // absolute start game screen
#define GAMEY 100  // absolute start geme screen
#define SCREENSX 480
#define SCREENSY 460

#define LOGOWIDTH 150
#define LOGOHEIGHT 55

// enemy 
#define BOMBSPEED 2
#define ENEMYSX  36
#define ENEMYSY   30
#define ENEMYROWX (GAMEX + ENEMYSX/2 )       // leave one 2* size on the side
#define ENEMYROWY (GAMEY + ENEMYSY/2 )        // leave one 2* size on the side
#define BOMBEND    (GAMEY + SCREENSY -1)         // the bottom
#define MAXENEMY   ( SCREENSX/(ENEMYSX*3/2))  // Max Enemy per row
#define MAXROWS     4
#define BUNKERLIMIT (BOMBEND - ENEMYSY*5)      // the dow limit for invaders

#define ZIGZAGTICK  29    // ticktime tick between zig zag        ! prime
#define DOWNTICK    413   // tick time tich between lower orbit   ! prime
#define ATTACKRATE  513   // tick time for next attack            ! prime

// Player
#define LASERSPEED 5
#define SKYLIMIT  GAMEY              // the SKY
#define PLAYERSX    26
#define PLAYERSY    36
#define PLAYERROWX  (GAMEX + SCREENSX/2)
#define PLAYERROWY  BOMBEND-PLAYERSY 
#define PLAYERENDX1 (GAMEX-1)
#define PLAYERENDX2 (GAMEX+SCREENSX-PLAYERSX+1)
 
#define ICONHEIGHT  55
#define ICONWIDTH   55

#define MINIX   12
#define MINIY   10

#define BOMBY 10
#define BOMBX 5

#define RAYY 10
#define RAYX 5

#define QRY 261
#define QRX 270


#define SPAVEINVADERSX 220
#define SPAVEINVADERSY 107

extern const uint16_t Ray[RAYY][RAYX];
extern const uint16_t Bomb[BOMBY][BOMBX];
extern const uint16_t BombE[BOMBY][BOMBX];
extern  const uint16_t laserexplode2[PLAYERSY][PLAYERSX];
extern  const uint16_t laserexplode[PLAYERSY][PLAYERSX];
extern  const uint16_t laser[PLAYERSY][PLAYERSX] ;
extern  const uint16_t LevelUp[LOGOHEIGHT][LOGOWIDTH];
extern  const uint16_t GameOver[LOGOHEIGHT][LOGOWIDTH];
extern const uint16_t LevelOver[LOGOHEIGHT][LOGOWIDTH]  ;
extern  const uint16_t STMicro[LOGOHEIGHT][LOGOWIDTH];
extern  const uint16_t invader4_2[ENEMYSY][ENEMYSX];
extern  const uint16_t invader4_1[ENEMYSY][ENEMYSX];
extern  const uint16_t invader3_2[ENEMYSY][ENEMYSX];
extern  const uint16_t invader3_1[ENEMYSY][ENEMYSX];
extern  const uint16_t invader2_2[ENEMYSY][ENEMYSX];
extern  const uint16_t invader2_1[ENEMYSY][ENEMYSX];
extern  const uint16_t invader1_2[ENEMYSY][ENEMYSX];
extern  const uint16_t invader1_1[ENEMYSY][ENEMYSX];
extern  const uint16_t Iexplode[ENEMYSY][ENEMYSX];
extern const uint16_t Arduino[LOGOHEIGHT][LOGOWIDTH] ;
extern const uint16_t STMicro2[LOGOHEIGHT][LOGOWIDTH] ;
extern const uint16_t BLE_on[ICONHEIGHT][ICONWIDTH];
extern const uint16_t BLE_off[ICONHEIGHT][ICONWIDTH];
extern const uint16_t shipcount[MINIY][MINIX];
extern const uint16_t SpaveInvaders[SPAVEINVADERSY][SPAVEINVADERSX] ;

#endif  //SCREEN_H