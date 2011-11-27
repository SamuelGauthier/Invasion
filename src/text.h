#ifndef TEXT_H
#define TEXT_H

#include "utils.h"
#include <stdarg.h>

//--------------------------------
// GText
//--------------------------------
struct GLetter{
	int x,y;
	int width,height;
	int xoffset, yoffset;

	int xadvance;
};

struct GFont{
	GLetter desc[128];

	GLuint texture;
	unsigned char* pixel;

	int w,h;
};

GFont* loadFont(const char* descfile);
void render(int posx, int posy, GFont* f, const char* s,...);
void renderMono(int posx, int posy, GFont* f, const char* fmt,...);
void releaseFont(void* f);

#endif
