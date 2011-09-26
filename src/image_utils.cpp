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

//--------------------------------------------------------
// loadTGA - charge une image du type .tga
//
// Voir TGA.txt pour plus d'informations
//--------------------------------------------------------
GImage* loadTGA(const char* filename)
{
	GImage* img = new GImage;
	
	uchar* beg = getContent(filename, 0);

	uchar idlength = *(beg);
	uchar type = *(beg + TGA_HD_IMAGE_TYPE);
	img->width = (uint)readShort(beg + TGA_HD_IMAGE_SPECS + 4);
	img->height = (uint)readShort(beg + TGA_HD_IMAGE_SPECS + 6);
	img->bpp = (*(beg + TGA_HD_IMAGE_SPECS + 8))/8;

	// La table des couleurs n'est pour l'instant par supportée par le loader
	assert(*(beg + TGA_HD_COLORMAP_TYPE) == 0);

	// Get the data offset
	uchar* data = beg + TGA_IMG_ID_FIELD + idlength + 0;

	// Store data
	uint datasize = img->width*img->height*(uint)(img->bpp);
	img->pixel = new uchar[datasize];

	// niveaux de gris ou couleurs
	if((type & 1) && (type & 2))
		img->grayscale = true;
	else
		img->grayscale = false;

	switch(type)
	{	
		case TGA_UNCOMP_RGB:
		case TGA_UNCOMP_BNW:
			memcpy(img->pixel, data, datasize);
			break;

		case TGA_RLE_RGB:
		case TGA_RLE_BNW:
			// décompresser 
			uchar* ptr;
			uchar size, type;
			ptr = img->pixel;

			while(ptr < img->pixel + datasize)
			{
				// Get the token
				size = (*data) & 0x7f;
				type = (*data) & 0x80;
				data++;

				for(uchar i = 0;i < size;i++)
				{
					for(ushort j = 0;j < img->bpp;j++)
						ptr[j] = data[j];

					ptr+=img->bpp;

					// raw packet
					if(!type)
					{
						data+=img->bpp;
					}
				}
			}
			break;

		default:
			break;
	}

	if(type == TGA_UNCOMP_RGB || type == TGA_RLE_RGB)
	{
		// échanger les rouges et les bleus BGR -> RGB BGRA -> RGBA
		uchar tmp;
		for(uint i=0;i < datasize;i+=(uint)(img->bpp))
		{
			tmp = img->pixel[i];
			img->pixel[i] = img->pixel[i+2];
			img->pixel[i+2] = tmp;
		}

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
	img->bpp = (uchar)readShort(header + BMP_HI_BPP)/8;
	img->grayscale = false;

	// Verify the file
	assert(readUint(header + BMP_HI_COMPRESSION) == 0);
	assert(img->bpp == 3);

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
	GLint format;
	glGenTextures(1, &texture_id);

	// Dire qu'on travaille avec cette texture
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// Remplir les données
	if(img->bpp == 1 && img->grayscale)
		format = GL_LUMINANCE;

	else if(img->bpp == 2 && img->grayscale)
		format = GL_LUMINANCE_ALPHA;

	else if(img->bpp == 3)
		format = GL_RGB;

	else if(img->bpp == 4)
	{
		format = GL_RGBA;
	}

	else
	{
		return 0;
	}
	
	glTexImage2D(GL_TEXTURE_2D,	
					 0,
					 (GLenum)format,
					 img->width,
					 img->height,
					 0,
					 format,
					 GL_UNSIGNED_BYTE,
					 img->pixel);


	return texture_id;
}

void releaseImage(GImage* img)
{
	delete[] img->pixel;
	delete img;
}
