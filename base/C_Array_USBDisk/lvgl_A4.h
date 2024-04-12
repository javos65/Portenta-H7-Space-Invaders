

// https://lvgl.io/tools/font_conv_v5_3

#ifndef LVGL_A4
#define LVGL_A4

#include "Arduino.h" 

typedef struct {
  const uint8_t *fontmap;  ///< Glyph bitmaps, concatenated
  const uint16_t *fontindex;  ///< Glyph bitmaps, concatenated
  uint16_t first;   ///< ASCII extents (first char)
  uint16_t last;    ///< ASCII extents (last char)
  uint8_t bpp; ///< Newline distance (y axis)
  uint8_t height;
  char name[24];
  char table[24];
} font4bpp;

extern font4bpp ArialRND12p;
extern const uint8_t  arial12pfonttable[];
extern const uint16_t arial12pindex[59][2];

extern font4bpp ArialRND18p;
extern const uint8_t  arial18pfonttable[];
extern const uint16_t arial18pindex[59][2];

extern font4bpp VantaFAT24p;
extern const uint8_t vantafat24pfontable[];
extern const uint16_t vantafat24pindex[11][2];

#endif //LVGL_A4