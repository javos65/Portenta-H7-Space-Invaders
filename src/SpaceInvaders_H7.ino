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
#include "USBC_Canvas.h"      // Graph functions for H7 USB-C Video
#include "Control.h"           // key control interface by touch or BLE
#include "SDebug.h"
#include "Level.h"

//This Game Engine based on object c++
#define null 0
//Create Controller object

REDIRECT_STDOUT_TO(Serial3)  


	void showScore(bool on);
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
	int m_levelScore;
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
   DBSERIALPORT.begin(38400);          // JV needed for debug interface can 
   Debugln("\n\n * Serial3 initialized");
   delay(5000); 
   Canvas_Init();                 // Init USB-C Video, 720x480 pixels via DSi : enables Video functions
   initgraphics_control(); 
   Debugln(" * Game Starting");
   
}

void loop()
{
int interval = 10; 
m_level = new Level(0,m_C);        // setup first level - create just one level, no renewing ! 
while(1){
  firstscreen();                                                // Show Splash screen and build Game screen
  newgame();                                                    // Reset Game counters
  showScore(true);                                  // Show score counters
  Debug("\n * New Game #");Debug(m_gamecounter++);
  digitalWrite(LEDR, HIGH);
  Canvas_DrawFrame();
  m_gameMillis = millis();                                      // init game timer
  m_level->renew(m_difficulty,m_C);                             // setup level
	while(!m_gameOver){                                           // play one game
    update();
		render();
    Canvas_DrawFrame();
    }
  if (m_score > m_highScore) { handle_highscore(); }            // handle game over : check high score, save high score
  //delete m_level;
  } // loop while(1)
}



void showScore(bool on){
  Canvas_DrawFrame();
	Canvas_FillRect( GAMEX, BOMBEND+4, LCDWIDTH-GAMEX-10, 30, BLACK);
  Printf_Canvas( GAMEX+10, BOMBEND+10,WHITE,4,"Level#%i:",m_difficulty); 
  Printf_Canvas( GAMEX + SCREENSX/2+20, BOMBEND+10,WHITE,4,"Score:"); 
  Canvas_FillRect( GAMEX, BOMBEND+1, SCREENSX, 3, CYAN);
  Printf_Canvas( GAMEX + SCREENSX-MINIX*4, BOMBEND+23,ORANGE,1,"SHIPS"); 
  for(int t =0 ; t<m_ships;t++) Canvas_DrawColors((int16_t)GAMEX + SCREENSX-MINIX*4 +t*(MINIX+8),(int16_t)BOMBEND+11,(int16_t) MINIX, (int16_t) MINIY,(uint16_t *) shipcount); 
  if ( (BOMBEND-(m_levelKills+m_kills)) >0 ) Canvas_FillRect(GAMEX-6, BOMBEND-(m_levelKills+m_kills), 3, m_levelKills+m_kills, RED);
   updateCounters();
}

// counters only
void updateCounters(){
  Canvas_FillRect( GAMEX+130, BOMBEND+10, 65, 30, BLACK);
  Printf_Canvas( GAMEX+130, BOMBEND+10,WHITE,4,"%i",m_levelScore); 
  Canvas_FillRect( GAMEX + SCREENSX/2+40 + 80, BOMBEND+10, 65, 30, BLACK);
  Printf_Canvas( GAMEX + SCREENSX/2+40 + 80, BOMBEND+10,CYAN,4,"%i",m_score); 
  Canvas_DrawFrame();
}


void levelComplete()
{
	Canvas_DrawColors((int16_t) GAMEX+SCREENSX/2-LOGOWIDTH/2,(int16_t)(GAMEY+SCREENSY/2)-20,(int16_t)LOGOWIDTH,(int16_t) LOGOHEIGHT, (uint16_t *)  LevelUp);
  Printf_Canvas( (GAMEX+SCREENSX/2)-40, (GAMEY+SCREENSY/2)+40,ORANGE,3,"Press Joystick"); 
  Printf_Canvas( (GAMEX+SCREENSX/2)-50, (GAMEY+SCREENSY/2)+60,ORANGE,3,"button to continue"); 
  Canvas_DrawFrame();
  Debugln(" * Level Complete");
  m_C->clearKeys();

	while ( m_C->getCS() != 1  && ( m_retimer!=0 && (millis()-m_retimer)<AUTORESTART )  ) {m_C->getKeys();;} // wait for key or timer

		m_difficulty++;
		Canvas_FillRect(GAMEX, GAMEY,LCDWIDTH-GAMEX-1,SCREENSY, BLACK); // clear players field
    m_levelScore=0;m_levelKills=0;
    m_retimer=0;
    randomize();
		m_level->renew(m_difficulty, m_C);
    showScore(true);
}


void levelOver()
{
	Canvas_DrawColors((int16_t) GAMEX+SCREENSX/2-LOGOWIDTH/2,(int16_t)(GAMEY+SCREENSY/2)-20,(int16_t)LOGOWIDTH,(int16_t) LOGOHEIGHT, (uint16_t *)  LevelOver);
  Printf_Canvas( (GAMEX+SCREENSX/2)-40, (GAMEY+SCREENSY/2)+40,YELLOW,3,"Press Joystick"); 
  Printf_Canvas( (GAMEX+SCREENSX/2)-60, (GAMEY+SCREENSY/2)+60,YELLOW,3,"button to try again"); 
  Canvas_DrawFrame();
  Debugln(" * Level Failed");
	m_C->clearKeys();

	while ( m_C->getCS() != 1  && ( m_retimer!=0 && (millis()-m_retimer)<AUTORESTART )  ) {m_C->getKeys();} // wait for key or timer

		Canvas_FillRect(GAMEX, GAMEY,LCDWIDTH-GAMEX-1,SCREENSY, BLACK); // clear players field
    m_levelScore=0;m_levelKills=0;
    m_retimer=0;
    randomize();
  	m_level->renew(m_difficulty, m_C);
    showScore(true);
}


void gameOver()
{
	Canvas_DrawColors((int16_t) GAMEX+SCREENSX/2-LOGOWIDTH/2,(int16_t)(GAMEY+SCREENSY/2)-20,(int16_t)LOGOWIDTH,(int16_t) LOGOHEIGHT, (uint16_t *)  GameOver);
  Printf_Canvas( (GAMEX+SCREENSX/2)-40, (GAMEY+SCREENSY/2)+40,YELLOW,3,"Press Joysticks"); 
  Printf_Canvas( (GAMEX+SCREENSX/2)-35, (GAMEY+SCREENSY/2)+60,YELLOW,3,"for new Game"); 
  Canvas_DrawFrame();
  Debugln(" * Game Over");digitalWrite(LEDR, LOW);
  m_C->clearKeys();

	while ( m_C->getCS() != 1  && ( m_retimer!=0 && (millis()-m_retimer)<AUTORESTART )  ) {m_C->getKeys();} // wait for key or timer

		Canvas_FillRect(GAMEX, GAMEY,LCDWIDTH-GAMEX-1,SCREENSY, BLACK); // clear players field
    m_levelScore=0;m_levelKills=0;
    m_retimer=0;
    showScore(true);
    m_gameOver = true; 
}


void update(){
  // check BLE connectivity and set Icon
  if ( m_C->getBLE() ) Canvas_DrawColors((int16_t) 40,(int16_t) 3*LOGOHEIGHT+20,(int16_t)ICONWIDTH,(int16_t) ICONHEIGHT, (uint16_t *)  BLE_on);
  else 	               Canvas_DrawColors((int16_t) 40,(int16_t) 3*LOGOHEIGHT+20,(int16_t)ICONWIDTH,(int16_t) ICONHEIGHT, (uint16_t *)  BLE_off);

	// update Level if game is not over
  if ( (!m_gameOver) && (m_level != null) ) {
      m_level->update();
      // keep track of score on the screen
      int sc = m_level->getScore();
      if( m_levelScore != sc ) {	
          m_levelKills = m_level->getKills();
  	      m_levelScore = sc;
          showScore(true);
          }
      } // if not game over
}

void render(){
int i,t;
if( !m_gameOver && (m_level != null) )
  {
	m_level->render();


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
          //Write_Highscore(m_highScore, m_highLevel,m_gameMillis,m_highKills);
          Canvas_DrawFrame();
          Canvas_DrawColors((int16_t) GAMEX+SCREENSX/2-SPAVEINVADERSX/2,(int16_t)(GAMEY+SCREENSY/2)-SPAVEINVADERSY,(int16_t)SPAVEINVADERSX,(int16_t) SPAVEINVADERSY, (uint16_t *)  SpaveInvaders);
          Printf_Canvas(GAMEX + SCREENSX/2-80,(GAMEY+SCREENSY/2)+SPAVEINVADERSY/4 + 10,CYAN,4,"HighScore : %i",m_highScore); 
          Printf_Canvas(GAMEX + SCREENSX/2-100,(GAMEY+SCREENSY/2)+SPAVEINVADERSY/4 + 35,CYAN,4,"%i Kills in %i:%i sec.",m_highKills,min,sec); 
          Canvas_DrawFrame();
          delay(5000);
}


void initgraphics_control(){
  m_C = new Control();
  m_C->init();

  // load high scores
  m_highScore=10;
  m_highLevel=1;
  m_highKills=8;
  //Read_Highscore(&m_highScore, &m_highLevel, &m_gameMillis,&m_highKills); // read Hisghcores from file
  randomize();
}

void newgame(){
	m_gameOver = false;
	m_score = 0;	
  m_kills = 0;
	m_currentMillis  = 0;
	m_previousMillis = 0;
	m_levelScore 	 = 0;	
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
  Canvas_FillScreen(BLACK);
  Canvas_DrawColors((int16_t) (LCDWIDTH-SPAVEINVADERSX)/2,(int16_t)(LCDHEIGHT-SPAVEINVADERSY)/2,(int16_t)SPAVEINVADERSX,(int16_t) SPAVEINVADERSY,(uint16_t *) SpaveInvaders);      // Splash Screen
  Canvas_DrawFrame();
  delay(4000);
   m_C->clearKeys();


  Canvas_FillScreen(BLACK);
  Canvas_DrawColors((int16_t) 5,(int16_t)LOGOHEIGHT,(int16_t)LOGOWIDTH,(int16_t) LOGOHEIGHT,(uint16_t *) STMicro);
  Canvas_DrawColors((int16_t) 5,(int16_t)2*LOGOHEIGHT+5,(int16_t)LOGOWIDTH,(int16_t) LOGOHEIGHT,(uint16_t *) STMicro2);
  Canvas_DrawColors((int16_t) 5,(int16_t)4*LOGOHEIGHT+25,(int16_t)LOGOWIDTH,(int16_t) LOGOHEIGHT,(uint16_t *) Arduino);
  Canvas_FillRect(GAMEX-12, GAMEY, 3, SCREENSY+24 , CYAN);
  Printf_Canvas(GAMEX+80,GAMEY/2+10,ORANGE,1,"HighScore at Level %i with %i points",m_highLevel,m_highScore); // print text on Canvas buffer, transparant (slower)

}

