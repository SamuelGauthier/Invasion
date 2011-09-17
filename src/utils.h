#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;

float Csqrt(float);
float Cabs(float);
uchar* getContent(const char* filename, uint* size);

void Bmp24Data(uchar* dst, const uchar* src,uint height, uint width);

ushort readShort(uchar*);
uint readUint(uchar*);

uint countchar(const char* str,char c);
void replacechar(char* str, char from, char by);
#endif
