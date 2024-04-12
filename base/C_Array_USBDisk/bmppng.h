/*****************************************************************************
* | File      	: BMPPNG.h
* | Function    :	Image definitions used for BMP / PNG to Array conversions
* | Info        : JV 2024
* | Github      : 
*----------------
* |	This version:   V2.0
* | Date        :   2024-03-16
* | https://github.com/javos65/H7-CArray-Converter-for-USBDisk
*
* Graphic definitions: Color modes, Colors, Image Structures
*
******************************************************************************/
#ifndef _BMPPNG_H
#define _BMPPNG_H
#include <stdint.h>

// from "stm32h7xx_hal_dma2d.h" // for Video definitions _ addeed RB swapped inputs
#define COLORMODE_ARGB8888        0x00000000U  /*!< ARGB8888 color mode */
#define COLORMODE_RGB888          0x00000001U  /*!< RGB888 color mode   */
#define COLORMODE_RGB565          0x00000002U  /*!< RGB565 color mode   */
#define COLORMODE_ARGB1555        0x00000003U  /*!< ARGB1555 color mode */
#define COLORMODE_ARGB4444        0x00000004U  /*!< ARGB4444 color mode */
#define COLORMODE_L8              0x00000005U  /*!< L8 color mode       */
#define COLORMODE_AL44            0x00000006U  /*!< AL44 color mode     */
#define COLORMODE_AL88            0x00000007U  /*!< AL88 color mode     */
#define COLORMODE_L4              0x00000008U  /*!< L4 color mode       */
#define COLORMODE_A8              0x00000009U  /*!< A8 color mode       */
#define COLORMODE_A4              0x0000000AU  /*!< A4 color mode       */
#define COLORMODE_YCBCR           0x0000000BU  /*!< YCbCr color mode */

#define COLORMODE_ABGR8888        0x00000010U  /*!< ARGB8888 color mode RB swapped */
#define COLORMODE_BGR888          0x00000011U  /*!< RGB888 color mode  RB swapped  */
#define COLORMODE_BGR565          0x00000012U  /*!< RGB565 color mode  RB swapped  */
#define COLORMODE_ABGR1555        0x00000013U  /*!< ARGB1555 color mode  RB swapped*/
#define COLORMODE_ABGR4444        0x00000014U  /*!< ARGB4444 color mode RB swapped */
#define COLORMODE_A4N             0x0000001AU  /*!< A4 color mode nibbel swapped   */

#define COLORMODE_XRGB8888        0x00000020U  /*!< ARGB8888 color mode alpha ignored*/
#define COLORMODE_XRGB1555        0x00000023U  /*!< ARGB1555 color mode alpha ignored */
#define COLORMODE_XRGB4444        0x00000024U  /*!< ARGB4444 color mode alpha ignored */

#define COLORMODE_XBGR8888        0x00000030U  /*!< ARGB8888 color mode alpha ignored RB swapped*/
#define COLORMODE_XBGR1555        0x00000033U  /*!< ABGR1555 color mode alpha ignored  RB swapped*/
#define COLORMODE_XBGR4444        0x00000034U  /*!< ABGR4444 color mode alpha ignored  RB swapped*/

// ALPHA REPLACEMENT VALUES
#define ZALPHAREPLACE   0x01000000
#define NOZALPHAREPLACE 0x00000000 // color to replace near alpha-0 pixels  is XRGB8888, with X > 0x01 (any ARGB888 color with non-zero transparancy)
#define ALPHANEARZERO  0x10       // Alpha limit where below its marked and zero-alpha, and replaced by the ALPHA-0 value
#define TRANSPARANT       true
#define NONTRANSPARANT    false
#define OPAQUE            false
#define REDBLUESWAP       true
#define NOREDBLUESWAP     false

// RGB565 colors
// online 565 colors https://rgbcolorpicker.com/565
// online 1555 colors https://roccoloxprograms.github.io/1555ColorPicker/
// online 444 colors : https://www.w3schools.com/colors/colors_picker.asp

#define CLEAR_565     0x0005
#define BLACK_565     0x0000
#define BLUE_565      0x001F
#define RED_565       0xF800
#define GREEN_565     0x07E0
#define LIME_565	    0xe7e0
#define CYAN_565      0x07FF
#define DARKCYAN_565  0x0DCB
#define MAGENTA_565   0xF81F
#define YELLOW_565    0xFFE0
#define WHITE_565     0xFFFF
#define ORANGE_565    0xfd20

#define WHITE_8888  0xFFFFFFFF // opaque color
#define BLACK_8888  0xFF000000 // opaque color
#define RED_8888    0xFFFF0000 // opaque color
#define GREEN_8888  0xFF00FF00 // opaque color
#define BLUE_8888   0xFF0000FF // opaque color

// Local Image Structure for Array conversion, incl textual names -> See Screen.h/Screen.cpp for the examples
typedef struct {
  void *imagearray;             /// pointer to array
  uint16_t width;               /// width
  uint16_t height;              /// height
  uint32_t colormode;           /// Alpha modes : COLORMODE_ARGB1555 , COLORMODE_ARGB4444 ,COLORMODE_ARGB8888 // Fixed color modes :  COLORMODE_RGB565
  uint8_t bpp;                  /// bits per pixel => /8 bytes per pixel
  uint16_t xpos;
  uint16_t ypos;
  char name[24];
  char namea[24];             // array name in text -
  char namesx[16];
  char namesy[16];
} h7image;

/* PNGDEC DEFINITIONS FOR ERROR AND PIXEL TYPE

// source pixel type
enum {
  PNG_PIXEL_GRAYSCALE=0,
    PNG_PIXEL_TRUECOLOR=2,
    PNG_PIXEL_INDEXED=3,
    PNG_PIXEL_GRAY_ALPHA=4,
    PNG_PIXEL_TRUECOLOR_ALPHA=6 < ---------------- only supported
};

// Error codes returned by getLastError()
enum {
    PNG_SUCCESS = 0,
    PNG_INVALID_PARAMETER,
    PNG_DECODE_ERROR,
    PNG_MEM_ERROR,
    PNG_NO_BUFFER,
    PNG_UNSUPPORTED_FEATURE,
    PNG_INVALID_FILE,
    PNG_TOO_BIG
};
*/

//
// BMP structure according  https://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
// BMP structure in picture https://upload.wikimedia.org/wikipedia/commons/7/75/BMPfileFormat.svg 
// file check : https://hexed.it/ 
//
typedef struct 
{
// Header                   //    Size  Offset Remark
  uint16_t signature;       // 	2 bytes	0000h	 'BM'
  uint32_t filesize;        // 	4 bytes	0002h	 File size in bytes
  uint32_t reserved;        //	4 bytes	0006h	 unused (=0)
  uint32_t dataoffset;      //	4 bytes	000Ah	 Offset from beginning of file to the beginning of the bitmap data
// Info header
  uint32_t headersize;	    //  4 bytes	000Eh	 Size of InfoHeader =40 
  uint32_t width;	          //  4 bytes	0012h	 Horizontal width of bitmap in pixels
  uint32_t height;	        //  4 bytes	0016h	 Vertical height of bitmap in pixels
  uint16_t planes;	        //  2 bytes	001Ah	 Number of Planes (=1)
  uint16_t bpp;             //  2 bytes	001Ch	 Bits per Pixel used to store palette entry information. 
                            //      1 = monochrome palette. NumColors = 1  
                            //      4 = 4bit palletized. NumColors = 16  
                            //      8 = 8bit palletized. NumColors = 256 
                            //      16 = 16bit RGB. NumColors = 65536
                            //      24 = 24bit RGB. NumColors = 16M
  uint32_t compression;	    //  4 bytes 001Eh  0 = BI_RGB   no compression  1 = BI_RLE8 8bit RLE encoding  2 = BI_RLE4 4bit RLE encoding
  uint32_t imagesize;	      //  4 bytes 0022h  (compressed) Size of Image  It is valid to set this =0 if Compression = 0
  uint32_t xpixelsperm;     //  4 bytes 0026h  horizontal resolution: Pixels/meter
  uint32_t ypixelsperm;     //  4 bytes 002Ah  vertical resolution: Pixels/meter
  uint32_t usedcolors;      //  4 bytes 002Eh  Number of actually used colors. For a 8-bit / pixel bitmap this will be 100h or 256
  uint32_t importantcolors; //  4 bytes 0032h  Number of actually used colors. For a 8-bit / pixel bitmap this will be 100h or 256
// Extended data for bitmasks
  uint32_t redmask;         // 4 bytes  0036h 
  uint32_t greenmask;       // 4 bytes  003Ah 
  uint32_t bluemask;        // 4 bytes  003Eh 
  uint32_t alphamask;       // 4 bytes  0042h 
  uint32_t colorspace;      // 4 bytes  0046h     
  // END OF HEADERS
  uint32_t buffercolor;     // MODIFIED colortype of image buffer : ARGB8888 XRGB8888 etc.
  uint8_t *imagebuffer;     // Pointer to Memory containing modified buffer image data - so excl padding, starting pixel(0,0)@(upper left), corrected Color-type
                            // 
} BMPstruct;


#endif //BMPPNG_H