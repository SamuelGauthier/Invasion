#ifndef UTILS_H
#define UTILS_H

typedef unsigned char uchar;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "image_utils.h"
#include "garbage_collector.h"

float Csqrt(float);
float Cabs(float);
uchar* getContent(const char* filename, uint* size);
bool isExtension(const char* filename, const char* extension);

void Bmp24Data(uchar* dst, const uchar* src,uint height, uint width);

ushort readShort(uchar*);
uint readUint(uchar*);

uint countchar(const char* str,char c);
void replacechar(char* str, char from, char by);

GLuint loadTexture(const char* filename);

#endif
