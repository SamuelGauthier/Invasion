#include "image_utils.h"
#include <assert.h>
#include <string.h>

GImage* loadImage(const char* filename)
{
	// BMP File Format
	if(isExtension(filename,"bmp"))
	{
		return loadBMP(filename);
	}

	else if(isExtension(filename,"tga"))
	{
		return loadTGA(filename);
	}

	else 
	{
		assert(0);
	}

	return NULL;
}

GImage* loadTGA(const char* filename)
{
	GImage* img = new GImage;
	
	uchar* beg = getContent(filename, 0);

	uchar idlength = *(beg);
	img->width = (uint)readShort(beg + TGA_HD_IMAGE_SPECS + 4);
	img->height = (uint)readShort(beg + TGA_HD_IMAGE_SPECS + 6);
	img->bpp = (ushort)*(beg + TGA_HD_IMAGE_SPECS + 8);

	// La table des couleurs n'est pour l'instant par supportée par le loader
	assert(*(beg + TGA_HD_COLORMAP_TYPE) == 0);

	// Get the data offset
	uchar* data = beg + TGA_IMG_ID_FIELD + idlength + 0;

	// Store data
	img->pixel = new uchar[img->width*img->height*(uint)(img->bpp/8)];
	memcpy(img->pixel, data, img->width*img->height*(uint)(img->bpp/8));

	// échanger les rouges et les bleus
	uchar tmp;
	for(uint i = 0;i < img->width*img->height*(uint)(img->bpp/8);i+=3)
	{
		tmp = img->pixel[i];
		img->pixel[i] = img->pixel[i+2];
		img->pixel[i+2] = tmp;
	}
	
	delete[] beg;
	return img;
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
