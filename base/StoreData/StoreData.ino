

#include <stdint.h>             // for uint definitions
#include <stdio.h>              // for FILE structure
#include "StoreData.h"
#include "DEBUGF.h"

void setup()
{

DEBUGFinit(115200);
delay(3000);
DEBUGF("Starting StoreDate Test\n\r");
InitStorage();

int hs, hl,k;
long t=millis();
DEBUGF("Storage initialized\n\r");

hs = 500; hl=5;k=200;
Write_Highscore(hs,hl,t,k);
DEBUGF("Written High scores %d,%d,%d time %l\n\r",hs,hl,k,t)

}

void loop()
{
int highs, highl,kill;
long time=millis();
Read_Highscore(&highs,&highl,&time,&kill);
DEBUGF("ReadHigh scores %d,%d,%d time %l\n\r",highs,highl,kill,time)
delay(3000);

highs=333;highl=2;time=millis();kill=2;
Write_Highscore(highs,highl,time,kill);
DEBUGF("Written High scores %d,%d,%d time %d\n\r",highs,highl,kill,time)
delay(3000);

Read_Highscore(&highs,&highl,&time,&kill);
DEBUGF("ReadHigh scores %d,%d,%d time %d\n\r",highs,highl,kill,time)
delay(3000);

while(1);;
}