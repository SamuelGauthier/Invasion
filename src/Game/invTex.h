#ifndef INVTEX_H
#define INVTEX_H

/*
	TODO:
		
	store the point size in invFont
	alpha component in textures
*/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>
#include "invMath.h"

GLuint cvtSDLSurfToGLTex(SDL_Surface* sdl, GLenum oformat = 0);
GLuint loadTexture(const char* filename);
SDL_Surface* interpolate(int* indices, int sub, SDL_Surface** surfs);
SDL_Surface* loadImage(const char* filename);

#endif
