#ifndef UI_H
#define UI_H
#include <GL/glut.h>
#include "utils.h"

//------------------------------------------------
// GLabel - En quelque sorte, le control universel
//
// posX, posY, sizeX, sizeY - 
class GLabel
{
	GLabel(const char* text, float _posX, float _posY, float _sizeX = 0.f, float _sizeY = 0.f);
	~GLabel();
	
	void OnMouse(bool click, int x, int y);
	void OnRender();

	float posX, posY;
	float sizeX, sizeY;

	bool stretch;
	float stretchSize;
};

#endif
