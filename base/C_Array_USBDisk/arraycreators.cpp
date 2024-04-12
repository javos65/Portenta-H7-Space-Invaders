#include <Arduino.h>
#include <stdint.h>             // for uint definitions
#include <stdio.h>              // for FILE structure
#include "lvgl_A4.h"            // for font4bpp structure
#include "bmppng.h"             // for image array and structures h7image
#include "pixelfunctions.h"     // Pixel conversion functions
#include "arraycreators.h"      // 

#include "DEBUGF.h"


// Wrapper function for all single array to array conversions incl file headers : DEFINE YOUR h7image correctly according your coloremode input!!!
uint8_t CreateArraytoArray(h7image image,uint32_t colortomode,bool transparant,uint32_t alphareplace)
{

  char CFileName[30];
  sprintf(CFileName,"ota/%s.h",image.name);
  FILE *CFile = fopen(CFileName, "w");
  fprintf(CFile,"// C-Array2Array IMAGE Converter by JV\n"); 
  fprintf(CFile,"// Required for STM32 ChromArt DMA2D support \n");
  fprintf(CFile,"https://github.com/javos65/H7-CArray-Converter-for-USBDisk\n\n");
  if(image.bpp == 32)  { // convert 32 bits(alpha) to 16bits - no Alpha, or 32 bits-Alpha
  switch (colortomode){ 
      case COLORMODE_ARGB1555 : Create1555array32(image, transparant, alphareplace, CFile) ; break;
      case COLORMODE_ARGB4444 : Create4444array32(image, transparant, alphareplace, CFile) ; break;
      case COLORMODE_ARGB8888 : Create8888array32(image, transparant, alphareplace, CFile) ; break;       // 32 to 32 bits output, usabel to convert RB or changhe Alpha channger
      case COLORMODE_RGB565   : Create565array32(image,  transparant, alphareplace, CFile) ; break; // no transparancy, all opaque, but fore mixing type
      default : break;
  }
  fclose(CFile);
  DEBUGF("  !Created Conversionfile from array: %s",CFileName);
  return (1);
  }
else {
    DEBUGF("  !Failed Conversionfile from array %s",CFileName);
  fclose(CFile);
  return(0);
  }
}



// Make your defines according your lvgl_A4 font names, A4 to A4 nibble swop
void ConvertA4_A4N(font4bpp font)
{
font4bpp myfont = font;
char CFileName[30];
sprintf(CFileName,"ota/%s.h",myfont.name);
FILE *CFile = fopen(CFileName, "w");
fprintf(CFile,"// A4 / 4bpp Alpha Font Format Converter by JV\n"); 
fprintf(CFile,"// Convert A4 Litte-endian to A4 Big endian type\n"); 
fprintf(CFile,"// Swops for every byte the upper and lower nibble\n"); 
fprintf(CFile,"// Required for STM32 ChromArt DMA2D support \n\n"); 
fprintf(CFile,"const uint8_t %s[] = {\n",myfont.table); 

char c=0;
uint16_t bytew,width,ip,cc,height = myfont.height;
uint8_t  byte4,*fmap= (uint8_t*) myfont.fontmap;
uint16_t *findex = (uint16_t*) myfont.fontindex;
uint16_t charcursor=0; 

for (c=myfont.first;c<=myfont.last;++c){
      ip =  *( (uint16_t*) findex + (c-myfont.first)*2 + 1);         // pointer to first pixel
      width= *( (uint16_t*) findex + (c-myfont.first)*2 + 0);        // number of pixels wide
      if (width%2) {bytew=(width+1)/2; }                       // odd pixels, stored 1x 4bpp more, compensate cursor move.
      else { bytew=width/2;}
       fprintf(CFile,"\n // Character \"%c/\" (0x%02x,d%i), Pixelsize %i*%i",c,c,c,width,height);      
       fprintf(CFile,"\n // Character \"%c\" (0x%02x,d%i), Size: %i bytes, Index d%i,0x%02x\n",c,c,c,bytew*height,ip,ip);
      for(int t=0;t<height;++t) {
        for(int i=0;i<bytew;++i) {
            byte4 = (uint8_t) *(fmap + ip + i + t*bytew ); byte4 = (uint8_t) ((byte4<<4)&0xF0) + ((byte4 >>4)&0X0F); // swop nibbles
            fprintf(CFile,"0x%02x,",byte4);
            }
          fprintf(CFile,"\n");  
        }
      }
  fprintf(CFile,"};"); 
  fclose(CFile);
  Serial.print("Created Conversionfile Font4bpp: ");  Serial.println(CFileName);
}



// 32 bits input to ARGB4444 is alpha based, if no alpha required, alpha/transparant should be false :  all alpha will be 0xff (100% on), if alpha0 ARGB8888color > 0x01000000 alpha-zero colors replaced by alpha0-color
void Create4444array32(h7image image, bool alpha, uint32_t alpha0,FILE *CFile)
{
uint32_t *p = (uint32_t *) image.imagearray;  

fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.8x,16,0,0, // Alpha mode COLORMODE_ARGB4444 \n",COLORMODE_ARGB4444); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input color mode for ARGB4444 output - 16 bits
          case  COLORMODE_ARGB8888 :  fprintf(CFile,"0x%04x,",Convert8888to4444((uint32_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ARGB8888 to ARGB4444
          case  COLORMODE_ABGR8888 :  fprintf(CFile,"0x%04x,",Convert8888to4444((uint32_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR8888 to ARGB4444 swap RB
          case  COLORMODE_XRGB8888 :  fprintf(CFile,"0x%04x,",Convert8888to4444((uint32_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XRGB8888 to ARGB4444
          case  COLORMODE_XBGR8888 :  fprintf(CFile,"0x%04x,",Convert8888to4444((uint32_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR8888 to ARGB4444 swap RB       
          default : break;         
          }
          }
          fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from [%0.8x] to ARGB4444, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}

// 32bits input to ARGB1555 is alpha based, if no alpha required, alpha = false all alpha will be 0xff (100% on), if alpha0 color > 0x01000000 (ARGB888color, value replaced by alpha0 lower byte value
void Create1555array32(h7image image, bool alpha, uint32_t alpha0, FILE *CFile)
{
uint32_t *p = (uint32_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.4x,16,0,0, // Alpha mode COLORMODE_ARGB1555\n",COLORMODE_ARGB1555); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 

fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input color mode for ARGB1555 output - 16 bits
          case  COLORMODE_ARGB8888 :  fprintf(CFile,"0x%04x,",Convert8888to1555((uint32_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ARGB8888 to ARGB1555
          case  COLORMODE_ABGR8888 :  fprintf(CFile,"0x%04x,",Convert8888to1555((uint32_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR8888 to ARGB1555 , swopping RB
          case  COLORMODE_XRGB8888 :  fprintf(CFile,"0x%04x,",Convert8888to1555((uint32_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XRGB8888 to ARGB1555, fixed Alpha
          case  COLORMODE_XBGR8888 :  fprintf(CFile,"0x%04x,",Convert8888to1555((uint32_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR8888 to ARGB1555, fixed Alpha, swopping RB           
          default : break;         
          }
            }
          fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from [%0.8x] to ARGB1555, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}


// 32bits input to ARGB8888 is alpha based, if no alpha required, alpha = false all alpha will be 0xff (100% on), if color > 0x01000000 value replaced by alpha0 lower byte value
void Create8888array32(h7image image,bool alpha, uint32_t alpha0, FILE *CFile)
{
uint32_t *p = (uint32_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"*0x0.8x,32,0,0, // Alpha mode ARGB8888 \n",COLORMODE_ARGB8888); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint32_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 

      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input colormode for ARGB888 output - 32 bits
          case  COLORMODE_ARGB8888 :  fprintf(CFile,"0x%08x,",Convert8888to8888((uint32_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ARGB8888 to ARGB8888
          case  COLORMODE_ABGR8888 :  fprintf(CFile,"0x%08x,",Convert8888to8888((uint32_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR8888 to ARGB8888 , swopping RB
          case  COLORMODE_XRGB8888 :  fprintf(CFile,"0x%08x,",Convert8888to8888((uint32_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XRGB8888 to ARGB8888, fixed Alpha
          case  COLORMODE_XBGR8888 :  fprintf(CFile,"0x%08x,",Convert8888to8888((uint32_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR8888 to ARGB8888, fixed Alpha, swopping RB           
          default : break;         
          }
            }
          fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from [%0.8x] to ARGB8888, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}


// 32bit input to RGB656 No Alpha format, any alpha-basedcolor will be created based on alpha0 color (must be > 0x01000000 / ARGB8888)
void Create565array32(h7image image, bool transparant, uint32_t alpha0,FILE *CFile)
{
uint32_t *p = (uint32_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.4x,16,0,0, // Alpha mode COLORMODE_RGB565 \n",COLORMODE_RGB565); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
            switch(image.colormode){ // check input color mode
            case  COLORMODE_ARGB8888 :  fprintf(CFile,"0x%04x,",   Convert8888to565( (uint32_t) *(p+i+t*image.width),false,transparant,alpha0)  );break;  // Convert ARGB8888 to RGB565
            case  COLORMODE_ABGR8888 :  fprintf(CFile,"0x%04x,",   Convert8888to565( (uint32_t) *(p+i+t*image.width),true, transparant, alpha0)  );break;  // Convert ABGR8888 to RGB565 swopping RB
            case  COLORMODE_XRGB8888 :  fprintf(CFile,"0x%04x,",   Convert8888to565( (uint32_t) *(p+i+t*image.width),false,false,alpha0)  );break;  // Convert XRGB8888 to to RGB565, hard alpha = ff, no mixing
            case  COLORMODE_XBGR8888 :  fprintf(CFile,"0x%04x,",   Convert8888to565( (uint32_t) *(p+i+t*image.width),true,false, alpha0)  );break;  // Convert XBGR8888 to to RGB565 swopping RB , hard alpha = ff, no mixing       
            default : break;         
            }          
          }
        fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from [%0.8x] to RGB565, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}

void printStructure(FILE *CFile)
{
  fprintf(CFile,"\n#include <stdint.h>\n\n");
  fprintf(CFile,"\ntypedef struct {\n");
  fprintf(CFile,"void *imagearray;  // array pointer\n");
  fprintf(CFile,"uint16_t width;    // width image\n");               
  fprintf(CFile,"uint16_t height;   // heigt image\n");             
  fprintf(CFile,"uint32_t colormode;// colormode DMA2D\n");     
  fprintf(CFile,"uint8_t bpp;       // bits per pixel\n");           
  fprintf(CFile,"uint16_t xpos;     // x position\n");   
  fprintf(CFile,"uint16_t ypos;     // y position\n");                  
  fprintf(CFile,"char name[24];     // name in text\n");
  fprintf(CFile,"} G_image;         // end structure\n\n");
}



// 16bits input to ARGB1555 is alpha based, if no alpha required, alpha = false all alpha will be 0xff (100% on), if alpha0 color > 0x01000000 (ARGB888color, value replaced by alpha0 lower byte value
void Create1555array16(h7image image, bool alpha, uint32_t alpha0, FILE *CFile)
{
uint16_t *p = (uint16_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.4x,16,0,0, // Alpha mode COLORMODE_ARGB1555\n",COLORMODE_ARGB1555); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input color mode for ARGB1555 output - 16 bits
          case  COLORMODE_RGB565 :  fprintf(CFile,"0x%04x,",Convert565to1555((uint16_t) *(p+i+t*image.width) , false) );break;  // Convert RGB565 to ARGB1555
          case  COLORMODE_BGR565 :  fprintf(CFile,"0x%04x,",Convert565to1555((uint16_t) *(p+i+t*image.width) , true) );break;  // Convert BGR565 to ARGB15555, swopping RB
          case  COLORMODE_ARGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to1555((uint16_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ABGR1555 to ARGB1555 
          case  COLORMODE_ABGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to1555((uint16_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR1555 to ARGB1555 , swopping RB
          case  COLORMODE_XRGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to1555((uint16_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XBGR1555 to ARGB1555 
          case  COLORMODE_XBGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to1555((uint16_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR1555 to ARGB1555 , swopping RB          
          case  COLORMODE_ARGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to1555((uint16_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ABGR4444 to ARGB1555 
          case  COLORMODE_ABGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to1555((uint16_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR4444 to ARGB1555 , swopping RB    
          case  COLORMODE_XRGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to1555((uint16_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XBGR4444 to ARGB1555 
          case  COLORMODE_XBGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to1555((uint16_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR4444 to ARGB1555 , swopping RB                 
          default : break;         
          }
            }
        fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from 16 bit[%0.8x] to ARGB1555, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}


// 16bits input to ARGB4444 is alpha based, if no alpha required, alpha = false all alpha will be 0xff (100% on), if alpha0 color > 0x01000000 (ARGB888color, value replaced by alpha0 lower byte value
void Create4444array16(h7image image, bool alpha, uint32_t alpha0, FILE *CFile)
{
uint16_t *p = (uint16_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.4x,16,0,0, // Alpha mode COLORMODE_ARGB4444\n",COLORMODE_ARGB4444); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input color mode for ARGB1555 output - 16 bits
          case  COLORMODE_RGB565 :  fprintf(CFile,"0x%04x,",Convert565to4444((uint16_t) *(p+i+t*image.width) , false) );break;  // Convert RGB565 to ARGB4444
          case  COLORMODE_BGR565 :  fprintf(CFile,"0x%04x,",Convert565to4444((uint16_t) *(p+i+t*image.width) , true) );break;  // Convert BGR565 to ARGB15555, swopping RB
          case  COLORMODE_ARGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to4444((uint16_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ABGR1555 to ARGB4444 
          case  COLORMODE_ABGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to4444((uint16_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR1555 to ARGB4444 , swopping RB
          case  COLORMODE_XRGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to4444((uint16_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XBGR1555 to ARGB4444 
          case  COLORMODE_XBGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to4444((uint16_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR1555 to ARGB4444 , swopping RB          
          case  COLORMODE_ARGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to4444((uint16_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ABGR4444 to ARGB4444 
          case  COLORMODE_ABGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to4444((uint16_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR4444 to ARGB4444 , swopping RB    
          case  COLORMODE_XRGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to4444((uint16_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XBGR4444 to ARGB4444 
          case  COLORMODE_XBGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to4444((uint16_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR4444 to ARGB4444 , swopping RB                 
          default : break;         
          }
            }
        fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from 16 bit[%0.8x] to ARGB4444, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}

// 16bits input to ARGB8888 is alpha based, if no alpha required, alpha = false all alpha will be 0xff (100% on), if alpha0 color > 0x01000000 (ARGB888color, value replaced by alpha0 lower byte value
void Create8888array16(h7image image, bool alpha, uint32_t alpha0, FILE *CFile)
{
uint16_t *p = (uint16_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.4x,16,0,0, // Alpha mode COLORMODE_ARGB4444\n",COLORMODE_ARGB4444); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input color mode for ARGB1555 output - 16 bits
          case  COLORMODE_RGB565 :  fprintf(CFile,"0x%04x,",Convert565to8888((uint16_t) *(p+i+t*image.width) , false) );break;  // Convert RGB565 to ARGB8888
          case  COLORMODE_BGR565 :  fprintf(CFile,"0x%04x,",Convert565to8888((uint16_t) *(p+i+t*image.width) , true) );break;  // Convert BGR565 to ARGB15555, swopping RB
          case  COLORMODE_ARGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to8888((uint16_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ABGR1555 to ARGB8888 
          case  COLORMODE_ABGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to8888((uint16_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR1555 to ARGB8888 , swopping RB
          case  COLORMODE_XRGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to8888((uint16_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XBGR1555 to ARGB8888 
          case  COLORMODE_XBGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to8888((uint16_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR1555 to ARGB8888 , swopping RB          
          case  COLORMODE_ARGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to8888((uint16_t) *(p+i+t*image.width) , false,alpha,alpha0) );break;  // Convert ABGR4444 to ARGB8888 
          case  COLORMODE_ABGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to8888((uint16_t) *(p+i+t*image.width) , true,alpha,alpha0) );break;  // Convert ABGR4444 to ARGB8888 , swopping RB    
          case  COLORMODE_XRGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to8888((uint16_t) *(p+i+t*image.width) , false,OPAQUE,alpha0) );break;  // Convert XBGR4444 to ARGB8888 
          case  COLORMODE_XBGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to8888((uint16_t) *(p+i+t*image.width) , true,OPAQUE,alpha0) );break;  // Convert XBGR4444 to ARGB8888 , swopping RB                 
          default : break;         
          }
            }
        fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from 16 bit[%0.8x] to ARGB8888, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}


// 16bits input to RGB565 is non alpha based, all alpha will be 0xff (100% on), if alpha0 color > 0x01000000 (ARGB888color), zero alpha value replaced by alpha0 lower byte value
void Create565array16(h7image image,bool transparant, uint32_t alpha0, FILE *CFile)
{
uint16_t *p = (uint16_t *) image.imagearray;  
fprintf(CFile,"#define %s %i\n",image.namesy, image.height); 
fprintf(CFile,"#define %s %i\n\n",image.namesx, image.width); 
fprintf(CFile,"G_image %s ={\n",image.name); 
fprintf(CFile,"(void*) %s,%s,%s,\n",image.namea,image.namesx,image.namesy); 
fprintf(CFile,"0x%0.4x,16,0,0, // Alpha mode COLORMODE_RGB565\n",COLORMODE_RGB565); 
fprintf(CFile,"\"%s\\0\",\n",image.name);
fprintf(CFile,"};\n\n"); 
fprintf(CFile,"const uint16_t %s[%s][%s] = {\n",image.namea,image.namesy,image.namesx); 
      for(int t=0;t<image.height;++t) {
        for(int i=0;i<image.width;++i) {
          switch(image.colormode){ // check input color mode for ARGB1555 output - 16 bits
          case  COLORMODE_RGB565 :  fprintf(CFile,"0x%04x,",Convert565to565((uint16_t) *(p+i+t*image.width) , false) );break;  // Convert RGB565 to RGB565
          case  COLORMODE_BGR565 :  fprintf(CFile,"0x%04x,",Convert565to565((uint16_t) *(p+i+t*image.width) , true) );break;  // Convert BGR565 to ARGB15555, swopping RB
          case  COLORMODE_ARGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to565((uint16_t) *(p+i+t*image.width) , false,transparant,alpha0) );break;  // Convert ABGR1555 to RGB565 
          case  COLORMODE_ABGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to565((uint16_t) *(p+i+t*image.width) , true, transparant,alpha0) );break;  // Convert ABGR1555 to RGB565 , swopping RB
          case  COLORMODE_XRGB1555 :  fprintf(CFile,"0x%04x,",Convert1555to565((uint16_t) *(p+i+t*image.width) , false,false,alpha0) );break;  // Convert XBGR1555 to RGB565 ,set Opaque
          case  COLORMODE_XBGR1555 :  fprintf(CFile,"0x%04x,",Convert1555to565((uint16_t) *(p+i+t*image.width) , true, false, alpha0) );break;  // Convert XBGR1555 to RGB565 , swopping RB  ,set Opaque        
          case  COLORMODE_ARGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to565((uint16_t) *(p+i+t*image.width) , false,transparant,alpha0) );break;  // Convert ABGR4444 to RGB565 
          case  COLORMODE_ABGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to565((uint16_t) *(p+i+t*image.width) , true, transparant,alpha0) );break;  // Convert ABGR4444 to RGB565 , swopping RB    
          case  COLORMODE_XRGB4444 :  fprintf(CFile,"0x%04x,",Convert4444to565((uint16_t) *(p+i+t*image.width) , false,false,alpha0) );break;  // Convert XBGR4444 to RGB565 set Opaque
          case  COLORMODE_XBGR4444 :  fprintf(CFile,"0x%04x,",Convert4444to565((uint16_t) *(p+i+t*image.width) , true, false,alpha0) );break;  // Convert XBGR4444 to RGB565 , swopping RB  ,set Opaque               
          default : break;         
          }
            }
        fprintf(CFile,"\n");  
        }
  fprintf(CFile,"};"); 
  fprintf(CFile,"// Converted from 16 bit[%0.8x] to RGB565, %d bytes\n\n",image.colormode, image.height*image.width*image.bpp/8);
}

