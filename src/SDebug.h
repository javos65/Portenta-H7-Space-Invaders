/*****************************************************************************
* | File      	:	SDebug.h
* | Function    :	debug with Serial print and println 
* | Info        : 	JV 2023
*----------------
* |	This version:   V1.0
* | Date        :   2023-01-16
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __SDEBUG_H
#define __SDEBUG_H

#include "stdio.h"

//#define DEBUG 1
#define DBSERIALPORT Serial3 //define your serial out -> Serial3 is extra uart on Portenta H7
#if DEBUG
	#define Debug(__info,...) DBSERIALPORT.print(__info,##__VA_ARGS__)
#else
	#define Debug(__info,...)
#endif

#if DEBUG
	#define Debugln(__info,...) DBSERIALPORT.println(__info,##__VA_ARGS__)
#else
	#define Debugln(__info,...)  
#endif

#endif
