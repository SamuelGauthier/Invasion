#ifndef INVTEX_H
#define INVTEX_H
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>

namespace Tex 
{
	GLuint convertFromSDLSurface(SDL_Surface* surf);
} /* Texture */

namespace Text
{
	struct INV_Font
	{
		GLuint tex;
		int w,h;
	};

	inline bool init()
	{
		if(TTF_Init() == -1){
			printf("TTF_Init: %s\n", TTF_GetError());
			return false;
		}	

		atexit(TTF_Quit);
		return true;
	}

	inline TTF_Font* loadFont(const char* fontname, int ptsize)
	{
		TTF_Font* font;
		font = TTF_OpenFont(fontname, ptsize);
		if (!font) {
			fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
			return NULL;
		}
	}

	inline void closeFont(TTF_Font* font)
	{
		TTF_CloseFont(font);
	}

	INV_Font* genTextures(TTF_Font* f);	
	void releaseFont(INV_Font* font);
	void renderText(const char* str, GLint x, GLint y, INV_Font* font);	
} /* Font */

#endif
