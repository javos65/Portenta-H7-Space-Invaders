/*****************************************************************************
* | File      	: .cpp
* | Function    :	for arduino port
* | Info        : JV 2024
*----------------
* |	This version:   V1.0
* | Date        :   2024-02-16
* | Port        :   Arduino Portenta H7 with usb-c and BLE support
* | Info        :   BLE version for remote control via Smartphone App
*
******************************************************************************/

#include "Arduino.h"
#include "mbed.h"
#include "Arduino_H7_Video.h"
#include "SDRAM.h"
#include "dsi.h"

#include <stdio.h>
#include <stdint.h>

#define sleep _sleep

#include "DEBUGF.h"         //  Serial debugging
#include "H7Canvas.h"       // Canvas definitions



Arduino_H7_Video display(GENERIC_RESX, GENERIC_RESY, USBCVideo);
uint32_t* ARGB8888Canvas = 0;
uint16_t* RGB565Canvas = 0;
uint32_t active,current,end,size;
char TBUF[80];

static DMA2D_HandleTypeDef dma2d;
static LTDC_HandleTypeDef  ltdc;

struct color {
    uint32_t b:8;
    uint32_t g:8;
    uint32_t r:8;
    uint32_t a:8;
};
struct color colors[256];

//convert
uint32_t rgb565to8888(uint16_t rgb565Pixel)
{
    uint32_t r =  ( (((uint32_t) rgb565Pixel << 8) & 0x00F80000) + 0x00070000 ) ;
    uint32_t g =  ( (((uint32_t) rgb565Pixel << 5) & 0x0000FC00) + 0x00000300 ) ;
    uint32_t b =  ( (((uint32_t) rgb565Pixel <<3 ) & 0x000000F8) + 0x00000007 ) ;  
    uint32_t a =  0xFF000000;
    return   (uint32_t) (a | r | g | b);
}

//convert
uint16_t argb8888to565(uint32_t argb8888Pixel)
{
    uint8_t blue  = (uint8_t) ((argb8888Pixel   ) & 0x000000FF);
    uint8_t green = (uint8_t) ((argb8888Pixel>>8) & 0x000000FF);
    uint8_t red  = (uint8_t) ((argb8888Pixel>>16)& 0x000000FF);
    uint16_t b = (blue >> 3) & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3) & 0x1f) << 11;
    return (uint16_t) (r | g | b);
}

static void InitCLUT(uint32_t * clut)
{
  uint32_t  red = 0, green = 0, blue = 0;
  uint32_t  i = 0x00;

  /* Color map generation */
  for (i = 0; i < 256; i++)
  {
    /* Generate red, green and blue values */
    red = (i * 8) % 256;
    green = (i * 6) % 256;
    blue = (i * 4) % 256;

    red = red << 16;
    green = green << 8;

    /* Store the 32-bit value */
    clut[i] = 0xFF000000 | (red + green + blue);
  }
}


void CanvasAll_Init()
{
  pinMode(LEDR, OUTPUT);pinMode(LEDB, OUTPUT);pinMode(LEDG, OUTPUT);
  digitalWrite(LEDR, HIGH); digitalWrite(LEDB, HIGH);  digitalWrite(LEDG, HIGH);    

  DEBUGFinit(38400);
  DEBUGF("\n\r* Serial initialized\n\r");        
  digitalWrite(LEDG, LOW);                                   // JV set green on

  display.begin();       
  current=(uint32_t) dsi_getCurrentFrameBuffer();active=(uint32_t) dsi_getActiveFrameBuffer(); end =(uint32_t) dsi_getFramebufferEnd();  
  if(active > current ) size = end-current;    
  else size = end-active;                                     
  //InitCLUT( (uint32_t *) colors);
  //dsi_configueCLUT( (uint32_t *) colors);                    // set CLU (needed ????
  RGB565Canvas= (uint16_t*)ea_malloc(GENERIC_RESX*GENERIC_RESY*2);  // Allocate Canvas using RGB565 - 2 bytes pp
  ARGB8888Canvas= (uint32_t*)ea_malloc(SPRITE_RESX*SPRITE_RESY*4);  // Allocate Canvas using ARGB8888 - 4 bytes pp
  dsi_lcdFillArea( (void *)dsi_getCurrentFrameBuffer(),GENERIC_RESX,GENERIC_RESY, 0x000000); dsi_drawCurrentFrameBuffer(); // black background
  dsi_lcdFillArea( (void *)dsi_getCurrentFrameBuffer(),GENERIC_RESX,GENERIC_RESY, 0x000000); dsi_drawCurrentFrameBuffer(); // black background
  Canvas_ClearFrame(0x0000);
  Sprite_ClearFrame(0x00000000);
  digitalWrite(LEDG, HIGH); DEBUGF("* Display initialized");             // JV init display, green off
    DEBUGF(" * Display set at %dx%d \n\r",dsi_getDisplayXSize(),dsi_getDisplayYSize()); 
    DEBUGF(" * Dsi FrameBuffers - Active:0x%0.8x Current:0x%0.8x\n\r",active,current); 
    DEBUGF(" * Dsi Buffers ending at 0x%0.8x, Size %d Kbytes\n\r",end,size/1024);
    DEBUGF(" * Canvas RGB565  initialized: %d Kbytes, starting at 0x%0.8x\n\r",(GENERIC_RESX*GENERIC_RESY*2)/1024, RGB565Canvas);   
    DEBUGF(" * Canvas ARGB8888 initialized:%d Kbytes, starting at 0x%0.8x\n\r",(SPRITE_RESX*SPRITE_RESY*4)/1024,ARGB8888Canvas);  
  for(int t=0; t<4 ; ++t) {dsi_drawCurrentFrameBuffer();delay(100);} // send frame buffers 
  delay(2000);
}

// transffer RGB565 canvas to DSI buffer, update or not
void CanvasAll_DrawFrames(bool update)
{
  dsi_lcdDrawImage((void*) RGB565Canvas, (void *)(dsi_getCurrentFrameBuffer()) ,GENERIC_RESX, GENERIC_RESY, DMA2D_INPUT_RGB565); // Draw CANVAS to DSI current buffer
  Sprite_DrawFrame();                                                                                                            // Draw SPRITE Canvas to DSI current buffer - blended
  if (update) dsi_drawCurrentFrameBuffer();                                                                                      // if update, send current Frame buffer to LTDC controller
}


// Send current DSI Frame buffer to LTDC controller
void CanvasAll_CurrentFrame()
{
  dsi_drawCurrentFrameBuffer();
}

// Wrapper TExt function to split out font sizes
void CanvasAll_Text(const char * txt, uint16_t x, uint16_t y,uint16_t color,uint8_t size,bool type){
  switch (size) {
    case 1 :   CanvasAll_Drawtext(type, txt, ArialRND12p, x, y, color);break;
    case 2 :   CanvasAll_Drawtext(type,  txt, ArialRND18p, x, y, color);break;
    case 3 :   CanvasAll_Drawtext(type,  txt, VantaFAT24p, x, y, color);break;
    default :  CanvasAll_Drawtext(type,  txt, ArialRND12p, x, y, color);break;
    }
}


// 8bbp font, capitals only 
void CanvasAll_Drawtext( bool canvas, const char *text, font4bpp myfont, uint16_t x, uint16_t y,uint16_t color565)
{
char c=0;
uint16_t wcomp,bytew,width,ip,cc,height = myfont.height;
uint8_t  *fmap= (uint8_t*) myfont.fontmap;
uint16_t *findex = (uint16_t*) myfont.fontindex;
uint16_t charcursor=0; 

cc=0;
//DEBUGF("Printing [%s]\n\r",text);
while( (c=text[cc]) !=0 ){
 if(c>myfont.last) c=c-32; // change small to large ascii
 if ( (c>=myfont.first) && (c<=myfont.last)){
      ip =  *( (uint16_t*) findex + (c-myfont.first)*2 + 1);         // pointer to first pixel
      width= *( (uint16_t*) findex + (c-myfont.first)*2 + 0);        // number of pixels wide
      //DEBUGF(" *Char [%c] w*h=%d*%d",c,width,height);
      if (width%2) {width+=1; bytew=width/2; wcomp=1;} // odd pixels, stored 1x 4bpp more, compensate cursor move.
      else { bytew=width/2;wcomp=0;}
      //DEBUGF(" * Character: %c points to index %d\n\r",c,ip); 
      if (canvas) Canvas_DrawFont( (void *) (fmap + ip),(uint16_t) x+charcursor,(uint16_t) y,(uint32_t) width, (uint32_t) height, (uint32_t) DMA2D_INPUT_A4,color565) ;
      else Sprite_DrawFont( (void *) (fmap + ip),(uint16_t) x+charcursor,(uint16_t) y,(uint32_t) width, (uint32_t) height, (uint32_t) DMA2D_INPUT_A4, color565) ;
      charcursor+= (width-wcomp);
  }
 cc++; if(cc>36) break;
 }

}

// Clear Canvas RGB565 frame with one RGB565 color
void Canvas_ClearFrame(uint16_t rgb565color){
   CanvasAll_FillBuffer((void *)(RGB565Canvas), GENERIC_RESX, GENERIC_RESY, 0, rgb565to8888(rgb565color),DMA2D_OUTPUT_RGB565);
}




//Wrapper function for G_Image drawing
void Canvas_DrawImage(G_image spriteimage)
{
switch( spriteimage.colormode) {
    case DMA2D_INPUT_ARGB8888 : Canvas_DrawImageAlpha( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height,spriteimage.colormode);break;
    case DMA2D_INPUT_ARGB4444 : Canvas_DrawImageAlpha( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height,spriteimage.colormode);break;
    case DMA2D_INPUT_ARGB1555 : Canvas_DrawImageAlpha( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height,spriteimage.colormode);break;
    case DMA2D_INPUT_RGB565   : Canvas_DrawImage565( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height);break;
    default : break;
    }
}

// DRAW IMAGE WITH OPAQUE RGB565 color image  to RGB565 Canvas by copying//
void Canvas_DrawImage565(void *image,uint16_t x, uint16_t y, uint16_t xw, uint16_t  yw)
{
  if ( ((x+xw) < GENERIC_RESX) || ((y + yw) < GENERIC_RESY) ) {
  dsi_lcdDrawImage((void*)image, (void *)(RGB565Canvas + x + y*GENERIC_RESX) ,xw, yw, DMA2D_INPUT_RGB565);
  }
}

// DRAW IMAGE WITH ALPHA CHANNEL ARGB888/4444/1555 color image  to RGB565 Canvas by blending//
void Canvas_DrawImageAlpha(void *pSrc, uint16_t x, uint16_t y,uint32_t xSize, uint32_t ySize,uint32_t colormode) {
#if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
  if ( (colormode != DMA2D_INPUT_ARGB8888) && (colormode != DMA2D_INPUT_ARGB4444) && (colormode !=DMA2D_INPUT_ARGB1555) )
  { DEBUGF("Not supported Color mode for Alpha blend to RGB565 by DMA2D %0.8x",colormode);; }
  else  if( (x+xSize)>Canvas_width() || (y+ySize)> Canvas_height())
    { DEBUGF(" * Image outside Canvas Frame");;  }
  else
  {
	uint16_t *pDst = (uint16_t*) (RGB565Canvas + x + y*GENERIC_RESX); // calculate destination location on Canvas
  dma2d.Instance = DMA2D;
	/* Configure the DMA2D Mode, Color Mode and output offset */
	dma2d.Init.Mode         = DMA2D_M2M_BLEND;
	dma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
	dma2d.Init.OutputOffset = GENERIC_RESX - xSize;
	if (pDst == NULL) { pDst = (uint16_t*) dsi_getCurrentFrameBuffer();}
	/* Foreground Configuration */
	dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	//dma2d.LayerCfg[1].InputAlpha = 0x00;
	dma2d.LayerCfg[1].InputColorMode = colormode;      // DRAW IMAGE WITH ALPHA CHANNEL ARGB888 color image //
	dma2d.LayerCfg[1].InputOffset = 0;
  dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  // Background
  dma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  dma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
  dma2d.LayerCfg[0].InputOffset = GENERIC_RESX - xSize;
	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if( (HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) && (HAL_DMA2D_ConfigLayer(&dma2d, 0) == HAL_OK) ) {
			if (HAL_DMA2D_BlendingStart(&dma2d, (uint32_t)pSrc, (uint32_t)pDst,  (uint32_t)pDst,xSize, ySize) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
  }// color mode ok ?
}


// DRAW SPRITE CANVAS as ARGB888 color image direct to Current DSI buffer//
void Sprite_DrawFrame() { 
#if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
uint16_t *pDst = (uint16_t*) (dsi_getCurrentFrameBuffer() + 2*SPRITE_SX + 2*SPRITE_SY*GENERIC_RESX); // calculate destination location on RGB565 / DSI
uint32_t *pSrc = (uint32_t*) ARGB8888Canvas; // calculate destination location on RGB565 / DSI
  dma2d.Instance = DMA2D;
	/* Configure the DMA2D Mode, Color Mode and output offset */
	dma2d.Init.Mode         = DMA2D_M2M_BLEND;
	dma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
	dma2d.Init.OutputOffset = GENERIC_RESX - SPRITE_RESX; // Desitnation - DSI current buffer
	/* Foreground Configuration */
	dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	//dma2d.LayerCfg[1].InputAlpha = 0x00;
	dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;      // DRAW sprite WITH ALPHA CHANNEL ARGB888 color image //
	dma2d.LayerCfg[1].InputOffset = 0;
  dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  // Background
  dma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;   
  dma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
  dma2d.LayerCfg[0].InputOffset = GENERIC_RESX - SPRITE_RESX; // background - CanvasRGB565
	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if( (HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) && (HAL_DMA2D_ConfigLayer(&dma2d, 0) == HAL_OK) ) {
			if (HAL_DMA2D_BlendingStart(&dma2d, (uint32_t)pSrc, (uint32_t)pDst,  (uint32_t)pDst, SPRITE_RESX, SPRITE_RESY) == HAL_OK) {
			//if (HAL_DMA2D_Start(&dma2d, (uint32_t)sprite, (uint32_t)pDst, xSize, ySize) == HAL_OK) {        
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
}


// DRAW FONT WITH ALPHA CHANNEL, INPUT A8 or A4 TYPE ALPHA ONLY //
void Canvas_DrawFont(void *pSrc, uint16_t x, uint16_t y,uint32_t xSize, uint32_t ySize, uint32_t fontmode, uint16_t color565) {
#if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
if( (x+xSize)>Canvas_width() || (y+ySize)> Canvas_height())
    { DEBUGF(" * Font outside Canvas Frame");;  }
else
{
uint32_t color8888 =  rgb565to8888(color565);
uint16_t *pDst = (uint16_t*) (RGB565Canvas + x + y*GENERIC_RESX); // calculate destination location on Canvas
  dma2d.Instance = DMA2D;

	/* Configure the DMA2D Mode, Color Mode and output offset */
	dma2d.Init.Mode         = DMA2D_M2M_BLEND;
	dma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
	dma2d.Init.OutputOffset = GENERIC_RESX - xSize;

	if (pDst == NULL) { pDst = (uint16_t*) dsi_getCurrentFrameBuffer();}

	/* Foreground Configuration */
	dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	dma2d.LayerCfg[1].InputAlpha = color8888;
	dma2d.LayerCfg[1].InputColorMode = fontmode;            // DRAW FONT WITH ALPHA CHANNEL, INPUT A8 or A4 TYPE ALPHA ONLY //
	dma2d.LayerCfg[1].InputOffset = 0;
  dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;

  // Background
  dma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  dma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
  dma2d.LayerCfg[0].InputOffset = GENERIC_RESX - xSize;

	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if( (HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) && (HAL_DMA2D_ConfigLayer(&dma2d, 0) == HAL_OK) ) {
			if (HAL_DMA2D_BlendingStart(&dma2d, (uint32_t)pSrc, (uint32_t)pDst,  (uint32_t)pDst,xSize, ySize) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
  } // check size
}


// DRAW FONT WITH ALPHA CHANNEL, INPUT A8 or A4 TYPE ALPHA ONLY //
void Sprite_DrawFont(void *pSrc, uint16_t x, uint16_t y,uint32_t xSize, uint32_t ySize, uint32_t fontmode, uint16_t color565) {
#if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
if( (x+xSize)>Sprite_width() || (y+ySize)> Sprite_height())
    { DEBUGF(" * Font outside Sprite Frame");;  }
else
{	
uint16_t *pDst = (uint16_t*) (ARGB8888Canvas + x + y*SPRITE_RESX); // calculate destination location on Canvas
uint32_t color8888 = rgb565to8888( color565);
  dma2d.Instance = DMA2D;

	/* Configure the DMA2D Mode, Color Mode and output offset */
	dma2d.Init.Mode         = DMA2D_M2M_BLEND;
	dma2d.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
	dma2d.Init.OutputOffset = SPRITE_RESX - xSize;

	if (pDst == NULL) { pDst = (uint16_t*) dsi_getCurrentFrameBuffer();}

	/* Foreground Configuration */
	dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	dma2d.LayerCfg[1].InputAlpha = color8888;
	dma2d.LayerCfg[1].InputColorMode = fontmode;            // DRAW FONT WITH ALPHA CHANNEL, INPUT A8 or A4 TYPE ALPHA ONLY //
	dma2d.LayerCfg[1].InputOffset = 0;
  dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;

  // Background
  dma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  dma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;
  dma2d.LayerCfg[0].InputOffset = SPRITE_RESX - xSize;

	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if( (HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) && (HAL_DMA2D_ConfigLayer(&dma2d, 0) == HAL_OK) ) {
			if (HAL_DMA2D_BlendingStart(&dma2d, (uint32_t)pSrc, (uint32_t)pDst,  (uint32_t)pDst,xSize, ySize) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
} // check size
}



void Canvas_FillRect(uint16_t x, uint16_t y,uint16_t xw, uint16_t yw,uint16_t rgb565color,uint8_t alpha)
{
  uint32_t *tmpbuffer;
  if ( ((x+xw) < GENERIC_RESX) || ((y + yw) < GENERIC_RESY) ) {
     tmpbuffer= (uint32_t*)ea_malloc(xw*yw*4);                                          // alloc temprary memory
    uint32_t color8888 = rgb565to8888(rgb565color); color8888 = (color8888&0x00ffffff) + ( (uint32_t) alpha<<24);
    CanvasAll_FillBuffer( (void *)tmpbuffer,xw,yw, 0,color8888,DMA2D_OUTPUT_ARGB8888 );   // create ARGB8888 image with alpha
    Canvas_DrawImageAlpha( (void *) tmpbuffer, x, y,xw, yw, DMA2D_INPUT_ARGB8888 ) ;   // blend it into the RGB565 canvas
    ea_free(tmpbuffer);
    }
}


// Clear Sprite Canvas with argb8888 color
void Sprite_ClearFrame(uint32_t ARGB888color) { 
     CanvasAll_FillBuffer((void *)(ARGB8888Canvas), SPRITE_RESX,SPRITE_RESY, 0, ARGB888color,DMA2D_OUTPUT_ARGB8888);
}

// Fill Screen buffer with one color
void CanvasAll_FillBuffer(void *pDst, uint32_t xSize, uint32_t ySize, uint32_t Offline, uint32_t ColorIndex,uint32_t ColorMode) {
  #if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
	/* Register to memory mode with ARGB8888 as color Mode */
	dma2d.Init.Mode         = DMA2D_R2M;
	dma2d.Init.ColorMode    = ColorMode;	//DMA2D_OUTPUT_ARGB8888
	dma2d.Init.OutputOffset = Offline;

	dma2d.Instance = DMA2D;

	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if(HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) {
			if (HAL_DMA2D_Start(&dma2d, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
}

void Sprite_FillRect(uint16_t x, uint16_t y,uint16_t xw, uint16_t yw,uint16_t rgb565color,uint8_t alpha)
{
  if ( ((x+xw) < SPRITE_RESX) || ((y + yw) < SPRITE_RESY) ) {
    uint32_t color8888 = rgb565to8888(rgb565color); color8888 = (color8888&0x00ffffff) + ( (uint32_t) alpha<<24);
    CanvasAll_FillBuffer( (void *)(ARGB8888Canvas+ x + y*SPRITE_RESX),xw,yw, SPRITE_RESX-xw,color8888,DMA2D_OUTPUT_ARGB8888 ); // offline size !!
    }
}


//Wrapper function for G_Image drawing to Sprite
void Sprite_DrawImage(G_image spriteimage)
{
switch( spriteimage.colormode) {
    case DMA2D_INPUT_ARGB8888 : Sprite_DrawImageAlpha( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height,spriteimage.colormode);break;
    case DMA2D_INPUT_ARGB4444 : Sprite_DrawImageAlpha( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height,spriteimage.colormode);break;
    case DMA2D_INPUT_ARGB1555 : Sprite_DrawImageAlpha( spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height,spriteimage.colormode);break;
    case DMA2D_INPUT_RGB565   : Sprite_DrawImage565(   spriteimage.imagearray, spriteimage.xpos,spriteimage.ypos,spriteimage.width, spriteimage.height);break;
    default : break;
    }
}


// DRAW IMAGE WITH OPAQUE RGB565 color image  to ARGB8888Canvas by copying//
void Sprite_DrawImage565(void *image,uint16_t x, uint16_t y, uint16_t xw, uint16_t  yw)
{
#if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
  if( (x+xw)>Sprite_width() || (y+yw)> Sprite_height())
    { DEBUGF(" * Image outside Sprite Frame");;  }
 else {
  uint32_t *pDst = (uint32_t*) (ARGB8888Canvas + x + y*SPRITE_RESX); // calculate destination location on DSI
	dma2d.Instance = DMA2D;
	/* Configure the DMA2D Mode, Color Mode and output offset */
	dma2d.Init.Mode         = DMA2D_M2M_PFC;
	dma2d.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
	dma2d.Init.OutputOffset = SPRITE_RESX - xw;
	/* Foreground Configuration */
	dma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
	dma2d.LayerCfg[1].InputAlpha = 0xFF;
	dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
	dma2d.LayerCfg[1].InputOffset = 0;

	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if(HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) {
			if (HAL_DMA2D_Start(&dma2d, (uint32_t) image, (uint32_t)pDst, xw, yw) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
 } // check size
}



// DRAW IMAGE WITH ALPHA CHANNEL ARG1555/4444/8888 color image to SPRITE CANVAS//
void Sprite_DrawImageAlpha( void* sprite, uint16_t x, uint16_t y,uint32_t xSize, uint32_t ySize,uint32_t colormode) { 
#if defined(__CORTEX_M7) 
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
#endif
  if ( (colormode != DMA2D_INPUT_ARGB8888) && (colormode != DMA2D_INPUT_ARGB4444) && (colormode !=DMA2D_INPUT_ARGB1555) )
  { DEBUGF(" * Not supported Color mode for Alpha blend to ARGB888 by DMA2D %0.8x",colormode);;  }
  else if( (x+xSize)>Sprite_width() || (y+ySize)> Sprite_height())
    { DEBUGF(" * Image outside Sprite Frame");;  }
  else
  {

  uint32_t *pDst = (uint32_t*) (ARGB8888Canvas + x + y*SPRITE_RESX); // calculate destination location on DSI
  dma2d.Instance = DMA2D;
	/* Configure the DMA2D Mode, Color Mode and output offset */
	dma2d.Init.Mode         = DMA2D_M2M_BLEND;
	dma2d.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888 ;
	dma2d.Init.OutputOffset = SPRITE_RESX - xSize; // Desitnation - DSI current buffer
	/* Foreground Configuration */
	dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	//dma2d.LayerCfg[1].InputAlpha = 0x00;
	dma2d.LayerCfg[1].InputColorMode =  colormode;      // DRAW sprite WITH ALPHA CHANNEL ARGB888 color image //
	dma2d.LayerCfg[1].InputOffset = 0;
  dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  // Background
  dma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;   
  dma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;
  dma2d.LayerCfg[0].InputOffset = SPRITE_RESX - xSize; // background - CanvasRGB565
	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&dma2d) == HAL_OK) {
		if( (HAL_DMA2D_ConfigLayer(&dma2d, 1) == HAL_OK) && (HAL_DMA2D_ConfigLayer(&dma2d, 0) == HAL_OK) ) {
			if (HAL_DMA2D_BlendingStart(&dma2d, (uint32_t)sprite, (uint32_t)pDst,  (uint32_t)pDst, xSize, ySize) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&dma2d, 25);
			}
		}
	}
  } // right color mode?
}

void Canvas_DrawImageR(G_image spriteimage,uint32_t rx,uint32_t ry)
{
G_image Timage;
uint32_t t,orgx = spriteimage.width;
uint32_t u,orgy = spriteimage.height;
//uint32_t * Dst = (uint32_t*) (ARGB8888Canvas + spriteimage.x + spriteimage.y*Sprite_width() ); // destination point

if (rx==0 ||ry==0) {
  if (rx==0 && ry==0) {rx=orgx;ry=orgy;} // if zero input -correct value
  else {
      if(rx==0) {rx=(orgx*ry)/orgy;} // resize rx to same size-relation
      if(ry==0) {ry=(orgy*rx)/orgx;} // resize ry to same size-relation
      }
  }

if(spriteimage.bpp == 16){
    uint16_t *TDst16 = (uint16_t*) ea_malloc(rx*ry*spriteimage.bpp/8);
DEBUGF(" * Tmp-Image size %dx%d starting at %0.8x",rx,ry,(uint32_t) TDst16);
    for ( t=0;t<ry;++t)
      for( u=0;u<rx;++u)
      {
        *( (uint16_t *) TDst16 + t*rx + u) = *( (uint16_t *) spriteimage.imagearray + ((t*orgy)/ry)*orgx + (u*orgx)/rx);
      }
   Timage.width=rx;Timage.height=ry;  Timage.bpp=spriteimage.bpp; Timage.xpos=spriteimage.xpos; Timage.ypos=spriteimage.ypos;
   Timage.imagearray = (void*) TDst16; Timage.colormode = spriteimage.colormode;
   Canvas_DrawImage(Timage);
   ea_free(TDst16);
   }


}

