#include "invTex.h"

GLuint cvtSDLSurfToGLTex(SDL_Surface* surf, GLenum oformat)
{
	GLuint tex;

	GLint nbOfColors = surf->format->BytesPerPixel;
	if(!oformat)
	{
		switch(nbOfColors)
		{
			case 4:
			if(surf->format->Rmask == 0x000000ff)
				oformat = GL_RGBA;
			else{
				oformat = GL_BGRA;
			}
			break;
		case 3:
			if(surf->format->Rmask == 0x000000ff)
				oformat = GL_RGB;
			else{
				oformat = GL_BGR;
			}
			break;
		case 1:
			oformat = GL_LUMINANCE;
			break;
		default:
			fprintf(stderr, "Could not convert an SDL_Surface to an GL Surface\n");
			return 0;
		}
	}

	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, nbOfColors, surf->w, surf->h, 0, oformat, GL_UNSIGNED_BYTE, surf->pixels);

	return tex;
}

SDL_Surface* loadImage(const char* filename)
{
	SDL_Surface* img = IMG_Load(filename);
	if(!img){
		fprintf(stderr, "Could not open %s\n", filename);
		return NULL;
	}

	return img;
}

GLuint loadTexture(const char* filename)
{
	SDL_Surface* img = loadImage(filename);
	if(!img)
		return 0;

	GLuint tex = cvtSDLSurfToGLTex(img);
	SDL_FreeSurface(img);

	return tex;
}

Uint8 coeff(Uint8 color, float c)
{
	return (Uint8)((float)color * c);
}

SDL_Surface* interpolate(int* indices, const int sub, SDL_Surface** surfs)
{
	SDL_Surface* dest = SDL_CreateRGBSurface(SDL_HWSURFACE, surfs[0]->w, surfs[0]->h, surfs[0]->format->BitsPerPixel, surfs[0]->format->Rmask, surfs[0]->format->Gmask, surfs[0]->format->Bmask, surfs[0]->format->Amask);

	SDL_LockSurface(dest);

	int w = surfs[0]->w, h = surfs[0]->h;
	Uint8* destp = (Uint8*)dest->pixels;
	const int texs_size = (sub+1)*(sub+1);
	Uint8** texs = new Uint8*[texs_size];
	for(int i=0;i<(sub+1)*(sub+1);i++)
		texs[i] = (Uint8*)surfs[indices[i]]->pixels;

	float subw = (float)(w/sub), subh = (float)(h/sub);

	for(int i=0;i<w;i++)
	{
		for(int j=0;j<h;j++)
		{
			float curx = (float)(i%(int)subw), cury = (float)(j%(int)subh);
			float ul = ((subw-curx-1.f)/(subw-1.f)) * ((subh-cury-1.f)/(subh-1.f));
			float ur = ((subw-curx-1.f)/(subw-1.f)) * (cury/(subh-1.f));
			float dl = (curx/(subw-1.f)) * ((subh-cury-1.f)/(subh-1.f));
			float dr = (curx/(subw-1.f)) * (cury/(subh-1.f));

			for(int k=0;k<surfs[0]->format->BytesPerPixel;k++)
			{
				int index = (i * w + j) * surfs[0]->format->BytesPerPixel + k;
				int ti = (i/(int)subw) * (sub+1) + (j/(int)subh);
				destp[index] = coeff(texs[ti][index], ul) + coeff(texs[ti+1][index], ur) + coeff(texs[ti+sub+1][index], dl) + coeff(texs[ti+sub+1+1][index], dr);
			}
		}
	}

	SDL_UnlockSurface(dest);

	delete[] texs;

	return dest;
}
