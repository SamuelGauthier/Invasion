#include "image_utils.h"
#include <assert.h>
#include <string.h>

GImage::GImage()
{
	pixel = NULL;
	width = height = 0;
	bpp = 0;
}

GImage::GImage(const char* filename)
{
	OnLoad(filename);
}

GImage::~GImage()
{
	OnRelease();
}

void GImage::OnRelease()
{
	if(pixel)
		delete[] pixel;
}

void GImage::OnLoad(const char* filename)
{
	char* buffer = new char[64];
	strcpy(buffer,filename);

	char* extension = buffer+1;
	while(*(extension-1) != '.')
		extension++;

	// BMP File Format
	if(strncmp(extension,"bmp",3) == 0)
	{
		OnLoadBMP(filename);
	}
	else 
	{
		assert(0);
	}

	delete[] buffer;
}

void GImage::OnLoadBMP(const char* filename)
{
	width = 30;
	height = 30;
	bpp = 24;

	uchar* beg = getContent(filename, 0);
    
	uchar* header = beg;
	
	// Verify that it's a bmp file
	assert(readShort(header + BMP_HF_SIGNATURE) == (ushort)('M'<<8 | 'B'));

	// Get Informations
	width = readUint(header + BMP_HI_WIDTH);
	height = readUint(header + BMP_HI_HEIGHT);
	bpp = readShort(header + BMP_HI_BPP);

	// Verify the file
	assert(readUint(header + BMP_HI_COMPRESSION) == 0);
	assert(bpp == 24);

	// Get Data
	uchar* data = (uchar*)(beg)+readUint(header + BMP_HF_DATA_OFFSET);
	pixel = new uchar[width*height*3];

	// Store and erase the padding
	Bmp24Data(pixel, data,height,width);

	// Release the buffer
	delete[] beg;
}
