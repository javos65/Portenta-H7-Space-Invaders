/*****************************************************************************
* | File      	:	StoreData.cpp
* | Function    :	Storing data on QSPI for C33 on LittleFileSystem
* | Info        : JV 2024
*----------------
* |	This version:   V2.0
* | Date        :   2024-01-16
* | Info        :   Basic version
*
* Game data store / read functions based on LittleFile system in QSPI
*
******************************************************************************/
#include "DEBUGF.h"
#include "StoreData.h"


#include "PluggableUSBMSD.h"
#include "QSPIFBlockDevice.h"
#include "MBRBlockDevice.h"
#include "FATFileSystem.h"

// FOR USB READABLE  DISK
static QSPIFBlockDevice root;
mbed::MBRBlockDevice wifi_data(&root, 1);
mbed::MBRBlockDevice ota_data(&root, 2);
static mbed::FATFileSystem wifi("wifi");
static mbed::FATFileSystem ota("ota");


void USBMSD::begin()
{
  int err = wifi.mount(&wifi_data);
  if (err) {
    while (!Serial);
    DEBUGF("Please run WiFiFirmwareUpdater / Partioner first");
    return;
  }
  ota.mount(&ota_data);
}
USBMSD MassStorage(&root);

void InitStorage()
{
  MassStorage.begin();
    //DEBUGF("Content of WiFi partition:");
    //printDirectory("/wifi");
    //DEBUGF("Content of OTA partition:");
    //printDirectory("/ota");
}


void printDirectory(char* name) {
  DIR *d;
  struct dirent *p;
  d = opendir(name);
  if (d != NULL) {
    while ((p = readdir(d)) != NULL) {
      DEBUGF(". %s",p->d_name);
    }
  }
  closedir(d);
}



// File Functions
FILE *mypfile;

void * myOpen(const char *filename, int32_t *size) {
  *size = 0;
  mypfile = fopen(filename, "r");
  if (mypfile != NULL){
      fseek(mypfile, 0L, SEEK_END); 
      uint32_t s = ftell(mypfile); rewind(mypfile);
      //DEBUGF("  Opened file %s with size %d bytes\n",filename,s);
      *size =  s;
      return (void *) mypfile;
      }
   else{ 
        DEBUGF("  File not found %s\n",filename);
        return NULL;
      }
}

void * myOpenW(const char *filename) {
  mypfile = fopen(filename, "w");
  if (mypfile != NULL){
      return (void *) mypfile;
      }
   else{ 
        DEBUGF("  Writefile  could not be opened %s\n",filename);
        return NULL;
      }
}

void myClose() {
  //DEBUGF("Closed file");
   fclose(mypfile);
}

int32_t myRead( uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    //DEBUGF("  Fileread %d"bytes, length);
     return (uint32_t) fread(buffer, 1, length, mypfile);
    }
  else return NULL; 
}


int32_t myWrite( uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    //DEBUGF("  Fileread %d"bytes, length);
     return (uint32_t) fwrite(buffer, 1, length, mypfile);
    }
  else return NULL; 
}

int32_t mySeek(int32_t position) {
  if (mypfile != NULL){    
      //DEBUGF("  Seek to %d",position);
      rewind(mypfile);
     return (uint32_t) fseek(mypfile,position,SEEK_CUR);
    }
  else return NULL;   
}



// Read Data and verify ID 
uint8_t Read_Data(struct EEData *O)
{
  int32_t fsize;
   myOpen(FILEHS,&fsize);
  if(mypfile != NULL) {
    uint8_t* ptr = (uint8_t*)O;       // make pointer to structure t
    fsize=myRead(ptr, (int32_t) sizeof(EEData));     myClose(); 
    DEBUGF(" * Data read from File system, with ID 0x%0.2x, size %d\n\r",O->identity,fsize);
    return(1);
  } // check file        
  else { 
    DEBUGF(" * File Read Failed : No File system\n\r");
    myClose(); 
    return(0);
    }         
}

// verify ID, verify FFS, Safe data, increase counter //
uint8_t Save_Data(struct EEData *O)
{
  uint8_t b;
  myOpenW(FILEHS);
  if(mypfile != NULL) {
     O->counter++;
     uint8_t* ptr = (uint8_t*) O;       // make pointer to structure O
     myWrite(ptr, (int32_t) sizeof(EEData));  myClose(); 
     DEBUGF(" * Saved Object to File system with ID %0.2x, size %i\n\r",O->identity,sizeof(EEData));
     return(1);
  }
  else DEBUGF(" *! File not open for writing\n\r");
 }


// ************ Callable functions from Game  ********************* //


EEData IV_Object = {99999999,100,1,EEData_ID,1,0}; // Global Variable Object structure example

 void Read_Highscore(int* HS, int * HL, long *T, int *K)
 {
// open file
 Read_Data(&IV_Object);
 *HS=IV_Object.highscore;*HL=IV_Object.highlevel;*T=IV_Object.time;*K=IV_Object.counter;
 }

  void Write_Highscore(int HS, int HL, long T, int K)
 {
// open myfile
 IV_Object.highscore=HS;IV_Object.highlevel=HL;IV_Object.time=T;IV_Object.counter=K;
 Save_Data(&IV_Object);
 }
