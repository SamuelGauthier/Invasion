#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "utils.h"
#include "file_struct.h"

struct GImage
{
	unsigned char* pixel;

	uint width;
	uint height;
	ushort bpp;
};

GImage* loadImage(const char* filename);
GImage* loadBMP(const char* filename);
GLuint allowGLTex(const GImage* img);
void releaseImage(GImage* img);

#endif
