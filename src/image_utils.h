#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "utils.h"
#include "file_struct.h"

class GImage 
{
public:
	GImage();
	GImage(const char* filename);
	~GImage();

	void OnLoad(const char* filename);
	void OnRelease();

	// Specific loader for each format
	void OnLoadBMP(const char*);

	unsigned char* pixel;
	
	uint width;
	uint height;
	ushort bpp;
};

#endif
