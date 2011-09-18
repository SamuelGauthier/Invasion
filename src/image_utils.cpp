#include "image_utils.h"
#include <assert.h>
#include <string.h>

GImage* loadImage(const char* filename)
{
	char* buffer = new char[64];
	strcpy(buffer,filename);

	char* extension = buffer+1;
	while(*(extension-1) != '.')
		extension++;

	// BMP File Format
	if(strncmp(extension,"bmp",3) == 0)
	{
		return loadBMP(filename);
	}

	else 
	{
		assert(0);
	}

	delete[] buffer;

	return NULL;
}

GImage* loadBMP(const char* filename)
{
	GImage* img = new GImage;

	uchar* beg = getContent(filename, 0);
	uchar* header = beg;
	
	// Verify that it's a bmp file
	assert(readShort(header + BMP_HF_SIGNATURE) == (ushort)('M'<<8 | 'B'));

	// Get Informations
	img->width = readUint(header + BMP_HI_WIDTH);
	img->height = readUint(header + BMP_HI_HEIGHT);
	img->bpp = readShort(header + BMP_HI_BPP);

	// Verify the file
	assert(readUint(header + BMP_HI_COMPRESSION) == 0);
	assert(img->bpp == 24);

	// Get Data
	uchar* data = (uchar*)(beg)+readUint(header + BMP_HF_DATA_OFFSET);
	img->pixel = new uchar[img->width*img->height*3];

	// Store and erase the padding
	Bmp24Data(img->pixel, data,img->height,img->width);

	// Release the buffer
	delete[] beg;

	return img;
}

GLuint allowGLTex(const GImage* img)
{
	if(!img->pixel)
	{
		return 0;
	}

	// Création d'une texture OpenGL
	GLuint texture_id;
	glGenTextures(1, &texture_id);

	// Dire qu'on travaille avec cette texture
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// Remplir les données
	if(img->bpp == 24)
	{
		glTexImage2D(GL_TEXTURE_2D,	
					 0,
					 GL_RGB,
					 img->width,
					 img->height,
					 0,
					 GL_RGB,
					 GL_UNSIGNED_BYTE,
					 img->pixel);
	}
	
	// Pour l'instant que le format 24 bits est supporté
	else
	{
		return 0;
	}

	return texture_id;
}

void releaseImage(GImage* img)
{
	delete[] img->pixel;
	delete img;
}
