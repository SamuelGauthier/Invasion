#ifndef TEXT_H
#define TEXT_H

#include <GL/glut.h>
#include "utils.h"

//--------------------------------
// GText
//--------------------------------
struct GLetter{
	float x,y;
	float width,height;
};

struct GFont{
	GLetter desc[128];

	GLuint texture;
	unsigned char* pixel;
};

GFont* loadFont(const char* descfile);
void render(GFont* f, const char* s);
void releaseFont(void* f);

#endif
