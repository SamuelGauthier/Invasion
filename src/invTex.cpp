#include "invTex.h"

//------------------------------------------------
// Textures utility
//------------------------------------------------
GLuint Tex::convertFromSDLSurface(SDL_Surface* surf)
{
	glEnable(GL_TEXTURE_2D);
	GLuint tex;
	GLenum format;

	// detect format
	GLint nbOfColors = surf->format->BytesPerPixel;
	switch(nbOfColors)
	{
		case 4:
			if(surf->format->Rmask == 0x000000ff)
				format = GL_RGBA;
			else
				format = GL_BGRA;
			break;
		case 3:
			if(surf->format->Rmask == 0x000000ff)
				format = GL_RGB;
			else
				format = GL_BGR;
			break;
		case 1:
			format = GL_LUMINANCE;
			break;
		default:
			fprintf(stderr, "Could not convert an SDL_Surface to an GL Surface\n");
			return 0;
	}
	
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, nbOfColors, surf->w, surf->h, 0, format, GL_UNSIGNED_BYTE, surf->pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	return tex;

}

//------------------------------------------------
// Font
//------------------------------------------------
Text::INV_Font* Text::genTextures(TTF_Font* f)
{
	if(!f)
		return NULL;

	INV_Font* font = new INV_Font;
	SDL_Rect rc;
	SDL_Surface* tmp, *dest;
	char i;
	char s[2] = {false};
	SDL_Color white = {255, 255, 255};
	SDL_Color black = {0, 0, 0};
	int j = 0;

	rc.x = 0; rc.y = 0;
	TTF_SizeText(f, "W", &font->w, &font->h);

	dest = SDL_CreateRGBSurface(SDL_HWSURFACE, font->w * 95, font->h, 24, 0, 0, 0, 0);

	// generate SDL_Surfaces
	for (i = 0; i < 95; i++)
	{
		s[0] = i + ' ';
		tmp = TTF_RenderText_Shaded(f, &s[0], white, black); 
		SDL_BlitSurface(tmp, NULL, dest, &rc);
		SDL_FreeSurface(tmp);

		rc.x += font->w;
	}

	// fill INV_Font
	font->tex = Tex::convertFromSDLSurface(dest);
	SDL_FreeSurface(dest);

	// close TTF_Font
	closeFont(f);
	return font;
}

void Text::releaseFont(INV_Font* font)
{
	glDeleteTextures(1, &font->tex);

	delete font;
}

void Text::renderText(const char* str, GLint x, GLint y, INV_Font* font)
{
	GLint w = (GLint)font->w, h = (GLint)font->h;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font->tex);
	glBegin(GL_QUADS);
	while(*str)
	{
		char s = *str;
		float a = (float)(s - ' ');
		
		glTexCoord2f(1.f/95.f * a, 0.0f);
		glVertex3i(x, y, 0.0f);

		glTexCoord2f(1.f/95.f * (a+1.f), 0.0f);
		glVertex3i(x + w, y, 0.0f);

		glTexCoord2f(1.f/95.f * (a+1.f), 1.0f);
		glVertex3i(x + w, y + h, 0.0f);

		glTexCoord2f(1.f/95.f * a, 1.0f);
		glVertex3i(x, y + h, 0.0f);

		x+=w;
		str++;
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

