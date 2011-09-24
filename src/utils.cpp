#include "utils.h"

float Cabs(float x)
{
	if(x>0.f)
		return x;
	return -x;
}

float Csqrt(float x)
{
 	long i;
    float x2, y;
	const float threehalfs = 1.5f;

	x2 = x * 0.5f;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return y;	
}

uint countchar(const char* str, char c)
{
	uint count = 0;

	while(*str++)
		if(*str == c){count++;}

	return count;
}

void replacechar(char* str, char from, char by)
{
	while(*str++)
		if(*str == from){*str = by;}
}

ushort readShort(uchar* value)
{
	return ((ushort)(*value) | ((ushort)*(value+1)<<8));
}

uint readUint(uchar* value)
{
	return (((uint)*value) | (((uint)*(value+1)) << 8) |
			(((uint)*(value+2)) << 16) | (((uint)*(value+3)<<24)));
}

uchar* getContent(const char* filename, uint* size)
{
	// open the file
	FILE* input = fopen(filename, "rb");
	assert(input != 0);

	// get the size of the file
	fseek(input, 0, SEEK_END);
	uint filesize = (uint)ftell(input);
	fseek(input, 0, SEEK_SET);

	// fill buffer
	uchar* buffer = new uchar[filesize];
	fread(buffer, 1, filesize, input);

	// close file
	fclose(input);

	// save the size
	if(size)
		*size = filesize;

	return buffer;
}

void Bmp24Data(uchar* dst, const uchar* src,uint height, uint width)
{
	uint padding = 0;
	for(uint i=0;i<height;i++)
	{
		for(uint j=0;j<width;j++)
		{
			for(uint k=0;k<3;k++)
			{
				dst[3*(j+width*i) + k] = src[3*(j+i*width)+(2-k)+padding];
			}
		}
		padding+=width%4;
	} 
}

// Petite fonction qui simplifie le chargement de textures OpenGL
GLuint loadTexture(const char* filename)
{
	GImage* img = loadImage(filename);
	GLuint texture_id = allowGLTex(img);

	delete img;
	return texture_id;
}

bool isExtension(const char* filename, const char* extension)
{
	char* buffer = new char[64];
	strcpy(buffer,filename);

	char* search = buffer;
	while(*(search+3))
		search++;

	if(*(search-1) != '.')
	{
		delete[] buffer;
		return false;
	}

	if(!strncasecmp(search,extension,3))
	{
		delete[] buffer;
		return true;
	}

	delete[] buffer;
	return false;
}
