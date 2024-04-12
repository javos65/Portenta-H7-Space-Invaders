#ifndef CREATORS_H
#define CREATORS_H
#include "bmppng.h"

void ConvertA4_A4N(font4bpp font);

uint8_t CreateArray2Array(h7image image, bool swaprb,uint32_t colormode,bool transparant,uint32_t alphareplace);
void printStructure(FILE *CFile);
void Create4444array32(h7image image, bool alpha, uint32_t alpha0,FILE *CFile);
void Create1555array32(h7image image, bool alpha, uint32_t alpha0, FILE *CFile);
void Create8888array32(h7image image, bool alpha, uint32_t alpha0,FILE *CFile);
void Create565array32(h7image image, bool transparant, uint32_t alpha0,FILE *CFile);

void Create1555array16(h7image image, bool alpha, uint32_t alpha0, FILE *CFile);
void Create4444array16(h7image image, bool alpha, uint32_t alpha0, FILE *CFile);
void Create8888array16(h7image image, bool alpha, uint32_t alpha0, FILE *CFile);
void Create565array16(h7image image, bool transparant,uint32_t alpha0, FILE *CFile);

#endif //CREATORS_H

