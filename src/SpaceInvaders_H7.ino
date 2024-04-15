/*****************************************************************************
* | File      	: Space Invaders Demo  / SpaceInvaders.ino
* | Function    :	Demo for Portenta H7  with HAT expansion board , USB-C Video and BLE remote control interface
* | Info        : JV 2024
* | Github      : https://github.com/javos65/Portenta-H7-Space-Invaders
*----------------
* |	This version:   V1.0
* | Date        :   2024-03-16
* | Port        :   Arduino Portenta C33 BLE support
* | Info        :   BLE version for remote control via Smartphone App
* | IOriginal   :   Objecrtoriented setup : https://github.com/YXHYX/arduino-space-invaders
*
*  Portenta Carrier:     Portenta Hat Carrier  SKU ASX00049 Barcode 7630049204041
*  Portenta C33 board :  Portenta C33          SKU ABX00074 Barcode 7630049203556 
*
*
******************************************************************************/
#include "H7Canvas.h"     // Graph functions for H7 USB-C Video
#include "Control.h"      // key control interface by touch or BLE
#include "DEBUGF.h"       // Debug messages
#include "Level.h"
#include "Screen.h"
#include "StoreData.h"    // Highscore storage on QSPi/ USB drive

//This Game Engine based on object c++
#define null 0
//Create Controller object

REDIRECT_STDOUT_TO(Serial3)  


	void showScore();
	void levelComplete();
	void levelOver();
  void gameOver();
  void render();
  void update();
  void newgame();
  void handle_highscore();
  void randomize();
	void initgraphics_control();
  void firstscreen();

//Create a Game object
  int m_gamecounter=0;
	int m_score;
  int m_kills;
	int m_levelScore=0;
  int m_levelKills;
	int m_highScore;
  int m_highLevel;
  int m_highKills;
	bool m_gameOver;
	int m_difficulty;
  int m_ships;
  uint32_t m_retimer;
	//time variables
	long m_currentMillis;
	long m_previousMillis;
  long m_gameMillis;
  Control *m_C;
  Level* m_level;


void setup()
{
   pinMode(LEDR, OUTPUT);pinMode(LEDB, OUTPUT);pinMode(LEDG, OUTPUT);
   digitalWrite(LEDR, HIGH); digitalWrite(LEDB, HIGH);  digitalWrite(LEDG, HIGH);    
   DEBUGFinit(38400);          // JV needed for debug interface can 
   delay(5000); 
   CanvasAll_Init();                 // Init USB-C Video, 720x480 pixels via DSi : enables Video functions
   initgraphics_control(); 
   DEBUGF(" * Game Starting\n\r");
   
}

void loop()
{
int interval = 10; 
m_level = new Level(0,m_C);        // setup first level - create just one level, no renewing ! 
while(1){
  firstscreen();                                                // Show Splash screen and build Game screen
  newgame();                                                    // Reset Game counters
  showScore();                                              // Show score counters
  DEBUGF("\n\r * New Game # %i\n\r",m_gamecounter++);
  digitalWrite(LEDR, HIGH);
  CanvasAll_DrawFrames(true);
  m_gameMillis = millis();                                      // init game timer
  m_level->renew(m_difficulty,m_C);                             // setup level
	while(!m_gameOver){                                           // play one game
    update();
		render();
    }
  if (m_score > m_highScore) { handle_highscore(); }            // handle game over : check high score, save high score
  //delete m_level;
  } // loop while(1)
}



void showScore(){
	Sprite_FillRect( Sprite_width()-120, BOMBEND+2, 100, Sprite_height()-BOMBEND-3, RED,ERASEMASK); // clear ship area
  for(int t =0 ; t<m_ships;t++) { ship.xpos = Sprite_width()-80+t*(ship.width+8);ship.ypos= BOMBEND + 6;Sprite_DrawImage(ship); } // draw remaining ship
  if ( (BOMBEND-(m_levelKills+m_kills)) >0 ) Sprite_FillRect(0, BOMBEND-1-(m_levelKills+m_kills), 3, m_levelKills+m_kills, RED,0xA0);
  updateCounters();
}

// counters only
void updateCounters(){
  Sprite_FillRect( 0, BOMBEND+3, 300, Sprite_height()-BOMBEND-3,RED,ERASEMASK); // clear Score area
  Printf_Sprite( 0,  0, YELLOW,2,"  \0"); 
  Printf_Sprite( 255,  BOMBEND+2, YELLOW,2,"%i\0",m_score); 
  Printf_Sprite( 100, BOMBEND+2,  YELLOW,2,"%i\0",m_levelScore); 
}


void levelComplete()
{
  LevelUp.xpos=Sprite_width()/2-LevelUp.width/2;LevelUp.ypos=Sprite_height()/2-20;Sprite_DrawImage(LevelUp);
  Printf_Sprite( (Sprite_width()/2)-70, (Sprite_height()/2)+40,ORANGE,2,"Press Joystick"); 
  Printf_Sprite( (Sprite_width()/2)-80, (Sprite_height()/2)+60,ORANGE,2,"button to continue"); 
  CanvasAll_DrawFrames(true);
  DEBUGF(" * Level Complete\n\r");
  m_C->clearKeys();

	while ( m_C->getCS() != 1  && ( m_retimer!=0 && (millis()-m_retimer)<AUTORESTART )  ) {m_C->getKeys();;} // wait for key or timer

		m_difficulty++;
		Sprite_ClearFrame(BLACK8888); // clear players field
    m_levelScore=0;m_levelKills=0;
    m_retimer=0;
    randomize();
		m_level->renew(m_difficulty, m_C);
    showScore();
}


void levelOver()
{
  LevelOver.xpos=Sprite_width()/2-LevelOver.width/2;LevelOver.ypos=Sprite_height()/2-20;Sprite_DrawImage(LevelOver);
  Printf_Sprite( (Sprite_width()/2)-70, (Sprite_height()/2)+40,YELLOW,2,"Press Joystick"); 
  Printf_Sprite( (Sprite_width()/2)-90, (Sprite_height()/2)+60,YELLOW,2,"button to try again"); 
  CanvasAll_DrawFrames(true);
  DEBUGF(" * Level Failed\n\r");
	m_C->clearKeys();

	while ( m_C->getCS() != 1  && ( m_retimer!=0 && (millis()-m_retimer)<AUTORESTART )  ) {m_C->getKeys();} // wait for key or timer

		Sprite_ClearFrame(BLACK8888); // clear players field
    m_levelScore=0;m_levelKills=0;
    m_retimer=0;
    randomize();
  	m_level->renew(m_difficulty, m_C);
    showScore();
}


void gameOver()
{
  GameOver.xpos=Sprite_width()/2-GameOver.width/2;GameOver.ypos=Sprite_height()/2-20;Sprite_DrawImage(GameOver);  
  Printf_Sprite( (Sprite_width()/2)-80, (Sprite_height()/2)+40,YELLOW,2,"Press Joysticks"); 
  Printf_Sprite( (Sprite_width()/2)-75, (Sprite_height()/2)+60,YELLOW,2,"for new Game"); 
  CanvasAll_DrawFrames(true);
  DEBUGF(" * Game Over\n\r");digitalWrite(LEDR, LOW);
  m_C->clearKeys();

	while ( m_C->getCS() != 1  && ( m_retimer!=0 && (millis()-m_retimer)<AUTORESTART )  ) {m_C->getKeys();} // wait for key or timer

		Sprite_ClearFrame(BLACK8888); // clear players field
    m_levelScore=0;m_levelKills=0;
    m_retimer=0;
    showScore();
    m_gameOver = true; 
}


void update(){
  // check BLE connectivity and set Icon at bottom screen
  if ( m_C->getBLE() ) {BLE_on.xpos = Sprite_x()/2-BLE_on.width/2 ;   BLE_on.ypos=Canvas_height()-2*BLE_on.height; Canvas_DrawImage(BLE_on);}
  else 	               {BLE_off.xpos =Sprite_x()/2-BLE_off.width/2  ; BLE_off.ypos=Canvas_height()-2*BLE_off.height; Canvas_DrawImage(BLE_off);}

	// update Level if game is not over
  if ( (!m_gameOver) && (m_level != null) ) {
      m_level->update();
      m_levelKills = m_level->getKills();
      m_levelScore = m_level->getScore()+9;
      } // if not game over
}

void render(){
int i,t;
if( !m_gameOver && (m_level != null) )
  {

  Sprite_ClearFrame(BLACK8888); // clear Sprite area, build it up
	m_level->render();            // render sprites enemy + playes + bombs + laser
  showScore();                  // render score
  CanvasAll_DrawFrames(true);   // send video

	if(!m_level->getPlayerAlive() )       // Player died   
	{
    if(m_retimer==0) {
      m_retimer = millis();  // start retimer
      m_ships--;
      updateCounters();
      }
		 if (m_levelScore>0) {                      // migrate score
        m_kills += m_levelKills; m_levelKills=0;  
        t=m_levelScore;
        for(i=0;i<t;++i){
          m_score++;delay(30);
          m_levelScore--;                       // update total score
          updateCounters();
          }
      }    
		if(m_ships <0 ) gameOver();      // if no more ships : game is over
    else levelOver();                      // next try
	}
	else if(m_level->getLevelCompleted() && m_level->getPlayerAlive() ) // Level completed 
	{
     if(m_retimer==0) m_retimer = millis();      // start retimer
		 if (m_levelScore>0) {                      // migrate score
        m_kills += m_levelKills; m_levelKills=0;
        t=m_levelScore;
        for(i=0;i<t;++i){
          m_score++;delay(30);
          m_levelScore--;                       // update total score
          updateCounters();;
          }
      }
    levelComplete();
	}

  } // if( !m_gameOver )
}

void handle_highscore(){
m_gameMillis  = millis()- m_gameMillis;                 // record game time
int min = (int) (m_gameMillis/(60*1000));
int sec = (int) ((m_gameMillis-(min*60*1000))/1000);
          m_highScore=m_score;
          m_highLevel=m_difficulty;
          m_highKills=m_kills;
          Write_Highscore(m_highScore, m_highLevel,m_gameMillis,m_highKills);
          Sprite_ClearFrame(0x80FF5500);
          SpaceInvaders.xpos = Sprite_width()/2-SpaceInvaders.width/2;  SpaceInvaders.ypos = Sprite_height()/3-SpaceInvaders.height/2; Sprite_DrawImage(SpaceInvaders);   
          Printf_Sprite(Sprite_width()/2-80,Sprite_height()/2 + 10,YELLOW,2,"HighScore : %i",m_highScore); 
          Printf_Sprite(Sprite_width()/2-100,Sprite_height()/2 + 35,CYAN,2,"%i Kills in %i:%i sec.",m_highKills,min,sec); 
          CanvasAll_DrawFrames(true);
          delay(5000);
}


void initgraphics_control(){
  m_C = new Control();
  m_C->init();

  // load high scores
  m_highScore=0;
  m_highLevel=1;
  m_highKills=8;
  InitStorage();
  Read_Highscore(&m_highScore, &m_highLevel, &m_gameMillis,&m_highKills); // read Hisghcores from file
  randomize();
}

void newgame(){
	m_gameOver = false;
	m_score = 0;	
  m_kills = 0;
	m_currentMillis  = 0;
	m_previousMillis = 0;
	m_levelScore 	 = 11;	
  m_levelKills = 0;
	m_difficulty 	 = 1;
  m_retimer = 0;
  m_ships = 3;
  randomize();
}

void randomize()
{
  long t=millis(); for(int i = 0 ; i< (t&0x00ff);++i )  randomSeed((analogRead(0)) ); // generate random seed
}

void firstscreen(){
long t,tt=millis();
  Canvas_ClearFrame(BLACK);
  Sprite_ClearFrame(BLACK8888);
  CanvasAll_DrawFrames(true);
  SpaceInvaders.xpos = Canvas_width()/2-SpaceInvaders.width/2;  SpaceInvaders.ypos = Canvas_height()/2-SpaceInvaders.height/2; Canvas_DrawImage(SpaceInvaders);      // Splash Screen
  CanvasAll_DrawFrames(true);
  delay(5000);
   m_C->clearKeys();


  Canvas_ClearFrame(BLACK);
  SpaceInvaders.xpos = 0;  SpaceInvaders.ypos =Sprite_y() ; Canvas_DrawImageR(SpaceInvaders, Sprite_x()-6,0);   
  STMicro.xpos=Sprite_x()/2-STMicro.width/2 ;STMicro.ypos= Sprite_y()+STMicro.height*2;Canvas_DrawImage(STMicro);
  STMicro2.xpos=Sprite_x()/2-STMicro2.width/2;STMicro2.ypos=Sprite_y()+STMicro2.height*3+5;Canvas_DrawImage(STMicro2);
  Arduino.xpos=Sprite_x()/2-Arduino.width/2;Arduino.ypos= Sprite_y()+ Arduino.height*4+5;Canvas_DrawImage(Arduino);
  Splash.xpos=Sprite_x();Splash.ypos= Sprite_y()-3;Canvas_DrawImageR(Splash,Sprite_width(),Sprite_height()+30);
  Printf_Canvas(Sprite_x()+140,Sprite_y()/2+10,ORANGE,1,"HighScore at Level %i with %i points",m_highLevel,m_highScore); // print text on Canvas buffer, transparant (slower)

  Canvas_FillRect(Sprite_x()-5, Sprite_y()-20, 3,Sprite_height()+40 , DARKCYAN, 0xFF);
  Canvas_FillRect( Sprite_x(), Sprite_y()+BOMBEND, Sprite_width(), 2, ORANGE,0xFF);                            // draw bomb end line
  for(t=0;t<Sprite_width()/4;++t)  Canvas_FillRect( Sprite_x()+random(3,Sprite_width()-6), Sprite_y()+BOMBEND-1, random(1,3), 1, ORANGE,random(0x80,0xFF));   
  Printf_Canvas( Sprite_x()+20,   Sprite_y()+BOMBEND+2,  WHITE,2,  "Level#%i:",m_difficulty);   // draw fixed text
  Printf_Canvas( Sprite_x()+190, Sprite_y()+BOMBEND+2,  WHITE,2,  "Score:"); 
  Printf_Canvas( Sprite_x()+Sprite_width()-120, Sprite_y()+BOMBEND+5,  ORANGE,1, "SHIPS"); 
  CanvasAll_DrawFrames(true);delay(100); CanvasAll_DrawFrames(true);
}

