
/*****************************************************************************
* | File      	:	StoreData.h
* | Function    :	Storing data on QSPI for H7 - fatfile system
* | Info        : JV 2024
*----------------
* |	This version:   V2.0
* | Date        :   2024-01-16
* | Info        :   Basic version
*
* Game data store / read functions based on LittleFile system in QSPI
*
******************************************************************************/

#ifndef __STOREDATA_H
#define __STOREDATA_H

#include <Arduino.h>

#define FILEHS "ota/highscore.bin"
#define EEData_ID 0xB0        // EEprom /FFS save ID for object to retrieve


struct EEData {         
long time;
int highscore;
int highlevel;
uint8_t identity;
int counter;
uint8_t stop; // dummy
};

extern void InitStorage();
extern uint8_t  Read_Data(struct EEData *O);
extern uint8_t  Save_Data(struct EEData *O);

extern void     Read_Highscore(int* HS, int* HL,long *T,int *K);
extern void     Write_Highscore(int  HS, int HL,long T, int K);


#endif