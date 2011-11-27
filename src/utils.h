#ifndef UTILS_H
#define UTILS_H

typedef unsigned char uchar;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define GL_GLEXT_PROTOTYPES
#define M_PI 3.14159f
#include <windows.h>
#include <SDL/SDL.h>
#include <GL/glew.h>

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
char* getRepertory(const char* filename);

void Bmp24Data(uchar* dst, const uchar* src,uint height, uint width);

ushort readShort(uchar*);
uint readUint(uchar*);

uint countchar(const char* str,char c);
void replacechar(char* str, char from, char by);

GLuint loadTexture(const char* filename);

void setTimer(long sec);
long getTicks();

void drawRepere();

#endif
