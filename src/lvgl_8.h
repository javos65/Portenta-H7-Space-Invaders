
#ifndef LVGL_8
#define LVGL_8

#include "Arduino.h" 

typedef struct {
  uint8_t *fontmap;  ///< Glyph bitmaps, concatenated
  uint16_t *fontindex;  ///< Glyph bitmaps, concatenated
  uint16_t first;   ///< ASCII extents (first char)
  uint16_t last;    ///< ASCII extents (last char)
  uint8_t bpp; ///< Newline distance (y axis)
  uint8_t height;
} font8bpp;

extern font8bpp ArialBold;
extern uint8_t  fonttable1[];
extern uint16_t fontindex1[90-32+1][2];


#endif //LVGL_8