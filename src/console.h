#ifndef CONSOLE_H
#define CONSOLE_H

#include "text.h"

struct GConsole
{
	int show_percentage;
	bool show;
};

GConsole* initConsole();
void toggleConsole(GConsole* c);
void render(GConsole* c, GFont* f);
void releaseConsole(void* c);

#endif
