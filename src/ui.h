#ifndef UI_H
#define UI_H
#include "utils.h"
#include "garbage_collector.h"
#include "text.h"

enum
{
	slide_bar = 1
};
//------------------------------------------------
// GLabel - En quelque sorte, le control universel
//
// posX, posY, sizeX, sizeY - 

struct GSlideBar
{
	int min,max;
	int value;
	
	GSlideBar* pNext;
};

struct GGenericControl
{
	uchar type;
	void* ptr;
	
	GGenericControl* pNext;
};

struct GControlManager
{
	GGenericControl* pHead;

	bool enable;
};


GControlManager* initControlManager(bool enable);
GSlideBar* addSlideBar(GControlManager* cm, int min, int max, int value);
void setMouse(GControlManager* cm, int x, int y, bool pressed);
void toggleVisibility();
void render(GControlManager* cm, GFont* f);
void releaseControlManager(void* cm);
void drawSquare(int x, int y, int width, int height, int depth);

#endif
