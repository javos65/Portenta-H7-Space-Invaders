/*
   QSPI as USB Mass Storage example -> c-array conversion

   This example shows how to expose a QSPIF BlockDevice (16MB external flash on the Portenta H7)
   as an USB stick. It can be adapted to any kind of BlockDevice (FlashIAP or either RAM via HeapBlockDevice)
   Before loading this example, make sure you execute WiFiFirmwareUpdater sketch
   to create and format the proper partitions.

Used to create a C-Array format converter:
Use your C-array in .h include file, convert and write to USB MAss Storage to read it back on your system for re-use
OR .. drop your png files on the usb drive and reset the board :)

Function 1: Convert all PNG/BMP files on the USB drive
Function 2: A4 Font format converter : Swop nibbles in the byte-wise data [7:4] and [3:0]
Function 3: C-array converter : Reduces ARGB8888 (32 bit) to ARGB15555 (16 bit) - 1 Alpha bit for transparancy blending or other formats
Function 4: read PNG or BMP files and convert to C-array by filename

Functions required for proper use of the STM32 ChromArt DMA2D functions, and online converters dont support these formats very well
Use the Portenta H7 USB Massstorage example to setup your drive properly using partition for Wifi and ota (standard setup)

https://github.com/javos65/H7-CArray-Converter-for-USBDisk

*/

#include "PluggableUSBMSD.h"
#include "QSPIFBlockDevice.h"
#include "MBRBlockDevice.h"
#include "FATFileSystem.h"
#include "SDRAM.h"   // needed fro large png files

#include "PNGdec.h"            // PNG decoder

#include "lvgl_A4.h"            // font arrays and structures
#include "screen.h"             // image array and structures examples
#include "bmppng.h"             // image definitions and structures ie h7image / BMPstructure
#include "pixelfunctions.h"     // Pixel conversion functions
#include "arraycreators.h"      // C-array file functions

#include "DEBUGF.h"

REDIRECT_STDOUT_TO(Serial3)     // JV Console DEBUG for stdio info from Doom library - need to initialise Serial3 in setup()

#define DRAMMAXX 480
#define DRAMMAXY 320

// FOR PNG DECODER
PNG png;           // png object
uint8_t *RAWIMAGE; // pointer to big fat SDRAM buffer
uint8_t pngcounter = 0;
h7image imageCONV; // local image structure used for array conversions

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




// FILE functions for PNG DECODER - Generic usable
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
void myClose(void *handle) {
  //DEBUGF("Closed file");
   fclose(mypfile);
}
int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    //DEBUGF("  Fileread %d"bytes, length);
     return (uint32_t) fread(buffer, 1, length, mypfile);
    }
  else return NULL; 
}
int32_t mySeek(PNGFILE *handle, int32_t position) {
  if (mypfile != NULL){    
      //DEBUGF("  Seek to %d",position);
      rewind(mypfile);
     return (uint32_t) fseek(mypfile,position,SEEK_CUR);
    }
  else return NULL;   
}
void PNGDraw(PNGDRAW *pDraw) {
uint16_t usPixels[36];
 // unused callback function
}
// extra read functions

// read 16 bits data, 2 bytes at tg
int32_t myRead16(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    //DEBUGF(" Read32 %d bytes",length);
    uint16_t *pointer =  (uint16_t *) buffer;
     return (uint32_t) fread(pointer, 2, length, mypfile);  // read 16 bits little endian file wise
    }
  else return NULL; 
}

int32_t myRead32(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    //DEBUGF(" Read32 %d bytes",length);
    uint32_t *pointer =  (uint32_t *) buffer;
     return (uint32_t) fread(pointer, 4, length, mypfile);  // read 32 bits little endian file wise
    }
  else return NULL; 
}

int32_t myRead24(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    //DEBUGF(" Read24 %d bytes",length);
     return (uint32_t) fread(buffer, 3, length, mypfile);  // read 32 bits little endian file wise
    }
  else return NULL; 
}

// read 32 bits Little Endian , force highest byte to FF
int32_t myRead32_FF(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    uint8_t c1,c2,c3,c4;
    uint32_t counter=0;
    for(uint32_t i= 0 ; i<length; ++i){
      fread(&c1, 1, 1, mypfile);fread(&c2, 1, 1, mypfile);                                              
      fread(&c3, 1, 1, mypfile);fread(&c4, 1, 1, mypfile);                                             
        *( buffer + i*4     ) = c1; 
        *( buffer + i*4 + 1 ) = c2;
        *( buffer + i*4 + 2 ) = c3; 
        *( buffer + i*4 + 3 ) = 0xFF;
        counter++; 
        }
    //DEBUGF(" Read32i %d long32b\n",length);
     return (uint32_t) counter;
    }
  else return NULL; 
}

// read 32 bits Big endian, force highest byte to FF
int32_t myRead32B_FF(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    uint8_t c1,c2,c3,c4;
    uint32_t counter=0;
    for(uint32_t i= 0 ; i<length; ++i){
      fread(&c1, 1, 1, mypfile);fread(&c2, 1, 1, mypfile);                                              
      fread(&c3, 1, 1, mypfile);fread(&c4, 1, 1, mypfile);                                             
        *( buffer + i*4     ) = c2; 
        *( buffer + i*4 + 1 ) = c3;
        *( buffer + i*4 + 2 ) = c4; 
        *( buffer + i*4 + 3 ) = 0xFF;
        counter++; 
        }
    //DEBUGF(" Read32i %d long32b\n",length);
     return (uint32_t) counter;
    }
  else return NULL; 
}

// map 24 bit to 32 bit with extra 0xFF for alpha
int32_t myRead24_32FF(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (mypfile != NULL){  
    uint8_t c1,c2,c3,c4;
    uint32_t counter=0;
    for(uint32_t i= 0 ; i<length; ++i){
      fread(&c1, 1, 1, mypfile);fread(&c2, 1, 1, mypfile);                                              
      fread(&c3, 1, 1, mypfile);//fread(&c4, 1, 1, mypfile);                                             
        *( buffer + i*4     ) = c1; 
        *( buffer + i*4 + 1 ) = c2;
        *( buffer + i*4 + 2 ) = c3; 
        *( buffer + i*4 + 3 ) = 0xFF;
        counter++; 
        }
     return (uint32_t) counter;
    }
  else return NULL; 
}







void setup() {
  Serial.begin(115200);
  Serial3.begin(38400);         // JV needed for debug interface can 
  delay(3000);
  SDRAM.begin();
  RAWIMAGE= (uint8_t*)SDRAM.malloc(DRAMMAXX*DRAMMAXY*4);  // Allocate Sdram image buffer using 480*200 pixels max
  Serial.print(" * Starting Conversion Program");
  DEBUGF(" * +Debugf functions");
  MassStorage.begin();
    //DEBUGF("Content of WiFi partition:");
    //printDirectory("/wifi");
    //DEBUGF("Content of OTA partition:");
    //printDirectory("/ota");
char myFileName[] = "ota/info.txt"; 
FILE *myFile = fopen(myFileName, "w");
fprintf(myFile,"Test file, to see how cool this is \r\n");
fprintf(myFile,"Function 1: Convert all png/bmp files on the USB drive\r\n");
fprintf(myFile,"Function 2: A4 Font format converter : Swop nibbles in the byte-wise data [7:4] and [3:0]\r\n");
fprintf(myFile,"Function 3: C-array converter : ie Reduces ARGB8888 (32 bit) to ARGB15555 (16 bit) - incl Alpha mod\r\n");
fprintf(myFile,"Function 4: Read single BMP / PNG files and convert to C-array by filename\r\n\r\n");
fprintf(myFile,"https://github.com/javos65/H7-CArray-Converter-for-USBDisk\r\n");
fclose(myFile);
  ota.unmount();delay(1000);
  ota.mount(&ota_data);
}

void printDirectory(char* name) {
  DIR *d;
  struct dirent *p;
  d = opendir(name);
  if (d != NULL) {
    while ((p = readdir(d)) != NULL) {
      DEBUGF(p->d_name);
    }
  }
  closedir(d);
}






void loop() {
  Serial.flush();
  Serial.print("\nPress key to start new conversions...");
  while(!Serial.available() ) ;
  Serial.read();

  ConvertAllBMP(COLORMODE_RGB565);
  ConvertAllPNG(COLORMODE_ARGB4444,TRANSPARANT,ZALPHAREPLACE);
  //ConvertPNG("test2_32b_alpha.png",COLORMODE_ARGB1555,NONTRANSPARANT,RED_8888);   
  //ConvertBMP("test2_32b.bmp",COLORMODE_RGB565);
  //ConvertBMP("test2_24b.bmp",COLORMODE_RGB565);
  //ConvertBMP("test2_16b565.bmp",COLORMODE_ARGB4444);
  //ConvertBMP("test2_16b1555.bmp",COLORMODE_RGB565);
  //ConvertBMP("test2_16b4444.bmp",COLORMODE_ARGB8888);
  //readBMP("test2_32b.bmp");
  //readBMP("test2_32b8888.bmp");
  //readBMP("test2_24b.bmp");
  //readBMP("test2_24b888.bmp");
  //readBMP("test2_16b.bmp");
  //readBMP("test2_16b1555.bmp");
  //readBMP("test2_16b4444.bmp");
  //readBMP("test2_16b565.bmp");
  //ConvertAllPNG(COLORMODE_ARGB4444,TRANSPARANT,BLACK_8888);
  //ConvertAllPNG(COLORMODE_ARGB8888,TRANSPARANT,NOZALPHAREPLACE);
  //ConvertPNG("test2_32b_alpha.png",COLORMODE_ARGB8888,NONTRANSPARANT,BLACK_8888);     // convert single png file to 4444
  //CreateArray2Array(invader12,NOREDBLUESWAP,COLORMODE_ARGB4444,TRANSPARANT,NOZALPHAREPLACE);     // convert image array to 4444
  //CreateArray2Array(invader11,NOREDBLUESWAP,COLORMODE_ARGB1555,TRANSPARANT,NOZALPHAREPLACE);     // convert image array to 1555
  //ConvertA4_A4N(VantaFAT24p);                             // convert font array :  A4 nibble swop
  Serial.print("\nConversions done ..."); delay(1000);
  Serial.flush();

}





//  scan for all .PNG files on the card and put them in a list, then convert
void ConvertAllBMP(uint32_t colormode) {
  // extern h7image imageCONV // global defined  
  int t,i,rc, arraycount,filecount = 0;
  char c,fname[24],list[16][24];
  char CFileName[30];
  char outputname[24];// sprintf buffer
  BMPstruct localbmp;
  DIR *d;
  struct dirent *p;
  d = opendir("/ota");
  if (d != NULL) {
     DEBUGF("\n\n  Found /ota directory, reading files\n"); 
    while ((p = readdir(d)) != NULL) {
      if (p->d_type == DT_REG) {          // regular file
          sprintf(fname,"%s",p->d_name);
          const int len = strlen(fname);
          if ( (len > 3 && strcmp(fname + len - 3, "bmp") == 0) ) {
              DEBUGF(".BMP Image : %s\n", fname); 
              sprintf(list[filecount],"%s",fname);   // save name incl direcory name = full path 
              filecount = filecount + 1;
          } // a BMP found
      } // a valid file found 
    } // a Directory-entry found
  } // Directory found
  closedir(d);
  if (filecount == 0) { DEBUGF("No .BMP files found");; }
  else
  {
      DEBUGF("  %d bmp-files found, start array conversion.\n\n", filecount); 
      sprintf(CFileName,"ota/All_BMP_%d.h",pngcounter++);       //create output file
      FILE *CFile = fopen(CFileName, "w");
        fprintf(CFile,"// C-array PBMP Converter by JV\n"); 
        fprintf(CFile,"// Required for STM32 ChromArt DMA2D support \n");
        printStructure(CFile);
        arraycount=0;
        for( i=0;i<filecount;++i) {
            DEBUGF("\n  #%d file %s, ",i+1, list[i]);
            BMPstruct localbmp;
            localbmp = readBMP(list[i]);
            if (localbmp.imagebuffer == NULL) { DEBUGF("\n\n  !%s not a valid BMP for conversion\n",fname);;}
            else {
                if (writeBMParray(list[i], &localbmp, colormode , CFile) ==1 ) { DEBUGF("  Added Conversion array to file: %s\n",CFileName); ++arraycount;}
                }
        }
    fclose(CFile);
    ota.unmount();delay(1000);  ota.unmount();delay(1000);
    ota.mount(&ota_data);  
    DEBUGF("\n  Created %d BMP Conversion arrays to file: %s",arraycount,CFileName);      
  }
}




// convert single file to array
void ConvertBMP(char * fname, uint32_t colormode)
{
char outputname[24];// sprintf buffer
char CFileName[30];
uint8_t t;
BMPstruct localbmp;
localbmp = readBMP(fname);
if (localbmp.imagebuffer == NULL) { DEBUGF("\n\n  !%s not a valid BMP for conversion\n",fname);;}
else {
    for( t=0;t<strlen(fname)-4;t++) {outputname[t]=fname[t];}outputname[t]=0;    // create item name without extension png 
    sprintf(CFileName,"ota/%s.h",outputname);
    //DEBUGF("\n  Opening output file %s\n",CFileName);
    FILE *CFile = fopen(CFileName, "w");
    fprintf(CFile,"// C-array BMP Converter by JV\n"); 
    fprintf(CFile,"// Required for STM32 ChromArt DMA2D support \n");
    fprintf(CFile,"// Input %d bpp colormode[%0.8x] \n",localbmp.bpp,localbmp.buffercolor); 
    printStructure(CFile);
    if (writeBMParray(fname, &localbmp, colormode , CFile) ==1 ) { DEBUGF("  Created Conversion array file: %s\n",CFileName); ;}
    else { DEBUGF("  !FAILED Conversion array file: %s\n",CFileName); fprintf(CFile,"// Conversion failed \n");;}         
    fclose(CFile);
    ota.unmount();delay(1000);  ota.unmount();delay(1000);
    ota.mount(&ota_data);  
  } // check valid BMP
}

uint8_t writeBMParray(char * fname, BMPstruct *mybmp, uint32_t colormode ,FILE *CFile)
{
char fullname[30],objectname[24];
const char *cpointer;
int t,i,rc,succes=0;
              sprintf(fullname,"ota/%s",fname);                                     // create full name, inclpath : fullname
              cpointer = (const char *) fullname;                                   // make const char pointer to full name
              for(t=0;t<strlen(fname)-4;t++) {objectname[t]=fname[t];}objectname[t]=0;    // create item name without extension png :  objectname
              DEBUGF("  ArrayConverting %s [%d x %d], %dbpp, pixeltype=%d \n", objectname, mybmp->width, mybmp->height, mybmp->bpp, mybmp->buffercolor); 
              if (mybmp->bpp!=32 && mybmp->bpp!=16) {DEBUGF("  !BMP type is not Truecolor 16 or 32 bits");}
              else if (mybmp->width>DRAMMAXX || mybmp->height>DRAMMAXY ) {DEBUGF("  ! BMP X*Y is too large");;}
              else {

                  // create structure for conversion
                  imageCONV.imagearray = (uint32_t *) mybmp->imagebuffer;                          // 32bit wide buffer ARGB8888
                  imageCONV.height=mybmp->height;
                  imageCONV.width=mybmp->width;
                  imageCONV.bpp = mybmp->bpp;
                  imageCONV.colormode=mybmp->buffercolor; // BMParray actual structure
                  sprintf(imageCONV.namea,"%s_array",objectname);      // arrayname
                  if (strlen(objectname) > 3 ) sprintf(imageCONV.name,"%s",objectname);
                  else sprintf(imageCONV.name,"%s__",objectname);      // item name long enough
                  for(t=0;t<strlen(objectname);t++) {if (objectname[t]>0x60) objectname[t]-=0x20;} objectname[t]=0;   // make name in capitals
                  sprintf(imageCONV.namesx,"%.7s_X",objectname);       // define X name
                  sprintf(imageCONV.namesy,"%.7s_Y",objectname);       // define y name
                  // structure complete
                  if (mybmp->bpp == 32) switch (colormode){ // BMP 32 bits is always buffer color mode xxxx8888 and  Alpha=0xFF Zero ALPHAREPLACE not possible
                      case COLORMODE_ARGB1555 : Create1555array32(imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break; // convert BMP from ARGB8888 to 1555 - no alpha
                      case COLORMODE_ARGB4444 : Create4444array32(imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break; // convert BMP from ARGB8888 to 4444- no alpha
                      case COLORMODE_ARGB8888 : Create8888array32(imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break; // convert BMP from ARGB8888 to 8888 - no alpha
                      case COLORMODE_RGB565   : Create565array32( imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break;  // convert with no alpha : BMP to 565
                      default : break;
                      }
                  if (mybmp->bpp == 16) switch (colormode){ // BMP 16 bits has 3 codings, and  alpha is always 100%
                      case COLORMODE_ARGB1555 : Create1555array16(imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break; // convert BMP from 16 bits to 1555 no alpha
                      case COLORMODE_ARGB4444 : Create4444array16(imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break; // convert BMP from 16 bits to 4444- no alpha
                      case COLORMODE_ARGB8888 : Create8888array16(imageCONV,OPAQUE, NOZALPHAREPLACE, CFile) ; break; // convert BMP from 16 bits to 8888- no alpha
                      case COLORMODE_RGB565   : Create565array16( imageCONV,OPAQUE, BLACK_8888, CFile) ; break;  // convert with no alpha : BMP from 16 bit to 565 - background mixed to black
                      default : break;
                      }                      
                  succes=1;    
                } // BMP correct type ?

return succes;
}



// read BMP file, setup bmp structure, adapt image mapping
BMPstruct readBMP(char * fname)
{
// extern h7image imageCONV // global defined
char c[4],BUF[128],objectname[24],fullname[32]; // sprintf buffer
sprintf(fullname,"ota/%s",fname);      // create full path name 
const char *cpointer = (const char *)fullname;
int t,i;
int32_t fsize;
BMPstruct bmp;
bmp.imagebuffer=NULL;
bmp.bpp = 0;
    myOpen((const char *) fullname, &fsize);

if (fsize > 0)
    {
      DEBUGF("\n  Opened BMP %s", fullname);  
      bmp.signature=0x00;bmp.filesize=0x00;
      mySeek(NULL,0);
      myRead(NULL, (uint8_t *) &bmp.signature, 2); // Big Endian
      myRead(NULL, (uint8_t *) &bmp.filesize, 6*4); // read filesize - height
      myRead(NULL, (uint8_t *) &bmp.planes, 2*2); // read  planes and bpp
      myRead(NULL, (uint8_t *) &bmp.compression, 6*4); // read compression till important colors
      DEBUGF("  Sig[%0.4x]Filesize[%d] [%d x %d]pixels, %d bpp, compression[%d] colors[%d] ImageSize %d bytes\n", bmp.signature, bmp.filesize, bmp.width, bmp.height, bmp.bpp, bmp.compression,bmp.usedcolors,bmp.imagesize); 
      DEBUGF("  Header[0x%0.4x]Off[0x%0.4x] ", bmp.headersize, bmp.dataoffset);  
      if (bmp.headersize>0x28) {  // read color bitmasks and color space type
              myRead(NULL, (uint8_t *) &bmp.redmask, 5*4); 
              DEBUGF("  MaskR[0x%0.8x]G[0x%0.8x]B[0x%0.8x]A[0x%0.8x] ColorSpace[0x%0.8x] \n", bmp.redmask, bmp.greenmask,bmp.bluemask,bmp.alphamask,bmp.colorspace);  
              }
      else {DEBUGF("  no Mask data\n");;}
      if (bmp.bpp!=32 && bmp.bpp!=16 && bmp.bpp!=24) {DEBUGF("  ! BMP type is not Truecolor 16, 24 or 32 bits");}
      else if (bmp.width>DRAMMAXX || bmp.height>DRAMMAXY ) {DEBUGF("  ! BMP X*Y is too large");;}
      else {

        mySeek(NULL, (uint32_t) bmp.dataoffset);
        if(bmp.bpp == 32 && bmp.compression == 0) {       // standard read results in XRGB8888, with X=alpha alway 0 <------------------------- !!!!
            uint32_t readbytes = 4 ;                      // reading 4 bytes per read - myRead32
            uint32_t linebytes = (bmp.width*bmp.bpp/8);   // bytes per line, read per line = linebytes/ readbytes
            uint32_t paddingbytes=0 ;                     // 32 bits pp always ends on 4 bytes
            DEBUGF("16B : readbytes %d, Linebytes %d padding %d",readbytes,linebytes,paddingbytes);
            for (i=bmp.height-1; i>=0;i--){
                    myRead32_FF(NULL, (uint8_t *) RAWIMAGE+i*linebytes, linebytes/readbytes); // pass 8-bit pointer, read filesize by 32 bits  , force alpha to 0xff
                }
             bmp.redmask = 0x00ff0000;bmp.greenmask = 0x0000ff00;bmp.bluemask = 0x000000ff;bmp.alphamask = 0xff000000; // -> Alpha = FF  / opaque
             bmp.buffercolor = COLORMODE_ARGB8888;   
             bmp.imagebuffer= (uint8_t*) RAWIMAGE;   
    DEBUGBUFFER_32H( (uint32_t *) RAWIMAGE,32,8); // Show first 32 bytes as test result - should be ARGB888 with zero Alpha with compression = 0
            }
        if(bmp.bpp == 32 && bmp.compression == 3) {       // Endian byte swap read : results in ABGR8888, with rb-swap and X=alpha always 0 , use generated bitmasks <--------------- !!!!!
            uint32_t readbytes = 4 ;                      // reading 4 bytes per read - myRead32
            uint32_t linebytes = (bmp.width*bmp.bpp/8);   // bytes per line, read per line = linebytes/ readbytes
            uint32_t paddingbytes=0 ;                     // 32 bits pp always ends on 4 bytes
            for (i=bmp.height-1; i>=0;i--){
                    myRead32B_FF(NULL, (uint8_t *) RAWIMAGE+i*linebytes, linebytes/readbytes); // pass 8-bit pointer, read filesize by 32 bits / Big Endian converted ,force alpha to 0xff
                }
             bmp.redmask = 0x00ff0000;bmp.greenmask = 0x0000ff00;bmp.bluemask = 0x000000ff;bmp.alphamask = 0xff000000; // override mask bits :  -> Alpha = FF  / opaque      
             bmp.buffercolor = COLORMODE_ARGB8888;                        
             bmp.imagebuffer= (uint8_t*) RAWIMAGE;    
    DEBUGBUFFER_32H( (uint32_t *) RAWIMAGE,32,8); // Show first 32 bytes as test result - should be ARGB888 with zero Alpha with compression = 0
            }            
        if(bmp.bpp == 16 && bmp.compression == 0) {        // standard read results in ARGB1555, with X=alpha alway 0 <------------------------- !!!!
            uint32_t readbytes = 2 ;                      // reading 1 bytes per read - myRead
            uint32_t linebytes = (bmp.width*bmp.bpp/8);   // bytes per line, read per line = linebytes/ readbytes
            uint32_t paddingbytes=0 ;                     // 32 bits pp always ends on 4 bytes
            if(linebytes%4 != 0   ) paddingbytes = 4-linebytes%4;     // always equal 4  or 2
            //DEBUGF("16B : readbytes %d, Linebytes %d padding %d",readbytes,linebytes,paddingbytes);
            for (i=bmp.height-1; i>=0;i--){
                    myRead16(NULL, (uint8_t *) RAWIMAGE+i*linebytes, linebytes/readbytes); // pass 8-bit pointer, read filesize by 16 bits  / 2 bytes Big Endian converted one full line
                    if (paddingbytes !=0) myRead(NULL,(uint8_t*) c,paddingbytes); // read x single bytes
                }
             bmp.redmask = 0x00007c00;bmp.greenmask = 0x000003e0;bmp.bluemask = 0x0000001f;bmp.alphamask = 0x00000080; // alpha mask : alpha ignore  
             bmp.buffercolor = COLORMODE_XRGB1555;                           
             bmp.imagebuffer= (uint8_t*) RAWIMAGE;      
      DEBUGBUFFER_16H( (uint16_t *) RAWIMAGE,128,16); // Show first 32 bytes as test result - should be ARGB1555 style data at compression = 0 
             //DEBUGBUFFER_B( (uint8_t *) RAWIMAGE,160); // Show first 32 bytes as test result - should be RGB565 style data
            }
        if(bmp.bpp == 16 && bmp.compression == 3) {       // standard read results in ARGB4444, with X=alpha alway 0  OR !!! RGB565 colors,  -> use generated bit-mask
            uint32_t readbytes = 2 ;                      // reading 1 bytes per read - myRead
            uint32_t linebytes = (bmp.width*bmp.bpp/8);   // bytes per line, read per line = linebytes/ readbytes
            uint32_t paddingbytes=0 ;                     // 32 bits pp always ends on 4 bytes
            if(linebytes%4 != 0   ) paddingbytes = 4-linebytes%4;    // always equal 4  or 2
            //DEBUGF("16B : readbytes %d, Linebytes %d padding %d",readbytes,linebytes,paddingbytes);
            for (i=bmp.height-1; i>=0;i--){
                    myRead16(NULL, (uint8_t *) RAWIMAGE+i*linebytes, linebytes/readbytes); // pass 8-bit pointer, read filesize by 16 bits  / 2 bytes Big Endian converted one full line
                    if (paddingbytes !=0) myRead(NULL,(uint8_t*) c,paddingbytes); // read x single bytes
                }
             //
             if (bmp.redmask==0xf00 && bmp.greenmask==0xf0 && bmp.bluemask==0x0f ) bmp.buffercolor = COLORMODE_XRGB4444;  // used read color masks to determine the colormode
             else bmp.buffercolor = COLORMODE_RGB565;
             bmp.imagebuffer= (uint8_t*) RAWIMAGE;      
    DEBUGBUFFER_16H( (uint16_t *) RAWIMAGE,128,16); // Show first 32 bytes as test result - should be ARGB1555 style data at compression = 0 
            }            
        if(bmp.bpp == 24 && bmp.compression == 0) {       // 24 to 32 bit mapping, resulting in XRGB8888 -> forced Alpha to FF
            uint32_t readbytes = 3 ;                      // reading 3 bytes per read - myRead24_32FF -> map to 32 bit
            uint32_t linebytes = (bmp.width*bmp.bpp/8);   // bytes per line, read per line = linebytes/ readbytes
            uint32_t linebytes2 = (bmp.width*32/8);       // write per 32 bits !
            uint32_t paddingbytes=0 ;                     // 32 bits pp always ends on 4 bytes
            if(linebytes%4 != 0   ) paddingbytes = 4-linebytes%4;    // padding bytes
            //DEBUGF("16B : readbytes %d, Linebytes %d padding %d",readbytes,linebytes,paddingbytes);
            for (i=bmp.height-1; i>=0;i--){
                    myRead24_32FF(NULL, (uint8_t *) RAWIMAGE+i*(linebytes2), linebytes/readbytes); // pass 8-bit pointer, read filesize by 16 bits  / 2 bytes Big Endian converted one full line / Alpha forced to 0xFF / opaque
                    if (paddingbytes !=0) myRead(NULL,(uint8_t*) c,paddingbytes); // read x single bytes
                }
            bmp.redmask = 0x00ff0000;bmp.greenmask = 0x0000ff00;bmp.bluemask = 0x000000ff;bmp.alphamask = 0xff000000; // overide mask : no alpha mask                
            bmp.bpp=32; // mapped to 32 bits  
            bmp.buffercolor = COLORMODE_XRGB8888;    //COLORMODE_ARGB8888;
            bmp.imagebuffer= (uint8_t*) RAWIMAGE;   
      DEBUGBUFFER_32H( (uint32_t *) RAWIMAGE,32,8); // Show first 32 bytes as test result - should be ARGB1555 style data at compression = 0 
            }

        } // valid BMP check       
    } // file has size
myClose(NULL);
return bmp;
}






//
//
// PNG / BMP file callable Conversions
//
//


//  scan for all .PNG files on the card and put them in a list, then convert
void ConvertAllPNG(uint32_t colormode,bool transparant,uint32_t alphareplace) {
  // extern h7image imageCONV // global defined  
  int t,i,rc, arraycount,filecount = 0;
  char c,fname[24],list[16][24],fullname[30],objectname[24];// sprintf buffer
  char CFileName[30];
  const char *cpointer;
  DIR *d;
  struct dirent *p;
  d = opendir("/ota");
  if (d != NULL) {
     DEBUGF("\n\n  Found /ota directory, reading files\n"); 
    while ((p = readdir(d)) != NULL) {
      if (p->d_type == DT_REG) {          // regular file
          sprintf(fname,"%s",p->d_name);
          const int len = strlen(fname);
          if ( (len > 3 && strcmp(fname + len - 3, "png") == 0) ) {
              DEBUGF(".PNG Image : %s\n", fname); 
              sprintf(list[filecount],"%s",fname);   // save name incl direcory name = full path 
              filecount = filecount + 1;
          } // a PNG found
      } // a valid file found 
    } // a Directory-entry found
  } // Directory found
  closedir(d);
  if (filecount == 0) { DEBUGF("No .PNG files found");; }
  else
  {
      DEBUGF("  %d png-files found, start array conversion.\n\n", filecount); 
      sprintf(CFileName,"ota/All_PNG_%d.h",pngcounter++);       //create output file
      FILE *CFile = fopen(CFileName, "w");
        fprintf(CFile,"// C-array PNG Converter by JV\n"); 
        fprintf(CFile,"// Required for STM32 ChromArt DMA2D support \n");
        fprintf(CFile,"// Red-Blue swop used : %i  \n",true); 
        printStructure(CFile);
        arraycount=0;
        for( i=0;i<filecount;++i) {
            DEBUGF("\n  #%d file %s, ",i+1, list[i]);
            if ( writePNGarray(list[i], colormode , transparant,alphareplace, CFile) ) ++arraycount;
            }
    fclose(CFile);
    ota.unmount();delay(1000);  ota.unmount();delay(1000);
    ota.mount(&ota_data);  
    DEBUGF("\n  Created %d PNG Conversion arrays to file: %s",arraycount,CFileName);      
  }
}


// write array C-structure to file input is RAWIMAGE buffer in 32-bit structure ABGR8888, alpha can be overidden, zero-alpha's replaced
uint8_t writePNGarray(char * fname, uint32_t colormode , bool transparant, uint32_t alphareplace, FILE* CFile)
{
char fullname[30],objectname[24];
const char *cpointer;
int t,i,rc,succes=0;

              sprintf(fullname,"ota/%s",fname);                                     // create full name, inclpath : fullname
              cpointer = (const char *) fullname;                                   // make const char pointer to full name
              for(t=0;t<strlen(fname)-4;t++) {objectname[t]=fname[t];}objectname[t]=0;    // create item name without extension png :  objectname

              rc = png.open((const char *) cpointer, myOpen, myClose, myRead, mySeek, PNGDraw);
              if (rc == PNG_SUCCESS) {
                DEBUGF("  ArrayConverting %s [%d x %d], %dbpp, pixeltype=%d, alpha=%d\n", objectname, png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType(),png.hasAlpha()); 
                if (png.getPixelType()!=6 || png.hasAlpha()!=1) {fprintf(CFile," // PNG %s : no TrueColor with Alpha: %d-%d" , cpointer, png.getPixelType(),png.hasAlpha() );DEBUGF("  ! PNG type is not Truecolor with Alpha");} 
                else if (png.getWidth()>DRAMMAXX || png.getHeight()>DRAMMAXY ) {fprintf(CFile," // PNG file exceeds max size %dx%d" , DRAMMAXX,DRAMMAXY);DEBUGF("  ! PNG X*Y is too large");}
                else 
                  {
                  png.setBuffer( (uint8_t *) RAWIMAGE); // receive decodedpng in sdram image buffer !!!! Red and blue are swopped ! ABGR8888, not ARGB8888
                  rc = png.decode(NULL, 0);
                  if (rc!=0) {fprintf(CFile," // PNG Conversion failure %d converting %s" , rc,cpointer);DEBUGF("  !PNG conversion failure, error : %d",rc);}
                  else {
          DEBUGBUFFER_32H( (uint32_t *) RAWIMAGE,32,8); // Show first 32 bytes as test result - 
                  // create structure for conversion
                  imageCONV.imagearray = (uint32_t *) RAWIMAGE;                          // 32bit wide buffer ARGB8888
                  imageCONV.height=png.getHeight();
                  imageCONV.width=png.getWidth();
                  imageCONV.bpp = png.getBpp()*4;
                  imageCONV.colormode=COLORMODE_ABGR8888; // PNGarray color structure of the to be converted image has RB swopped !
                  sprintf(imageCONV.namea,"%s_array",objectname);      // arrayname
                  if (strlen(objectname) > 3 ) sprintf(imageCONV.name,"%s",objectname);
                  else sprintf(imageCONV.name,"%s__",objectname);      // item name long enough
                  for(t=0;t<strlen(objectname);t++) {if (objectname[t]>0x60) objectname[t]-=0x20;} objectname[t]=0;   // make name in capitals
                  sprintf(imageCONV.namesx,"%.7s_X",objectname);       // define X name
                  sprintf(imageCONV.namesy,"%.7s_Y",objectname);       // define y name
                  // structure complete
                  switch (colormode){
                      case COLORMODE_ARGB1555 : Create1555array32(imageCONV, transparant, alphareplace, CFile) ; break; // convert with red blue swop : png delivers ABGR8888 format
                      case COLORMODE_ARGB4444 : Create4444array32(imageCONV, transparant, alphareplace, CFile) ; break; // convert with red blue swop : png delivers ABGR8888 format
                      case COLORMODE_ARGB8888 : Create8888array32(imageCONV, transparant, alphareplace, CFile) ; break; // convert with red blue swop : png delivers ABGR8888 format
                      case COLORMODE_RGB565   : Create565array32( imageCONV, transparant, alphareplace, CFile) ; break;  // convert with red blue swop : png delivers ABGR8888 format
                      default : break;
                      }
                  succes=1;    
                  } // Conversion  ok ?
                } // PNG correct type ?
              png.close();
              }
return succes;
}


// convert single file to array
void ConvertPNG(char * fname, uint32_t colormode,bool transparant,uint32_t alphareplace)
{
char outputname[24];// sprintf buffer
char CFileName[30];
uint8_t t;
    for( t=0;t<strlen(fname)-4;t++) {outputname[t]=fname[t];}outputname[t]=0;    // create item name without extension png 
    sprintf(CFileName,"ota/%s.h",outputname);
    DEBUGF(" \n  Opening output file %s\n",CFileName);
    FILE *CFile = fopen(CFileName, "w");
    fprintf(CFile,"// C-array PNG Converter by JV\n"); 
    fprintf(CFile,"// Required for STM32 ChromArt DMA2D support \n");
    fprintf(CFile,"// Red-Blue swop used : %i  \n",true); 
    printStructure(CFile);
    if (writePNGarray(fname, colormode , transparant,alphareplace, CFile) ==1 ) { DEBUGF("  Created Conversion array file: %s\n",CFileName); ;}
    else { DEBUGF("  !FAILED Conversion array file: %s\n",CFileName);;}         
    fclose(CFile);
    ota.unmount();delay(1000);  ota.unmount();delay(1000);
    ota.mount(&ota_data);  
}





