#ifndef TEXT_H
#define TEXT_H

#include <GL/glut.h>

//--------------------------------
// GText
//--------------------------------
struct GText {

	unsigned char* pixel;
};

void initText(GText* t);
void renderText(const char* s, const float posX, const float posY);
#endif
