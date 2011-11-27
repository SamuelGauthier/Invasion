#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_X 100
#define CONSOLE_Y 30
#include "text.h"

struct GCommand
{
	GCommand* pNextRow;
	GCommand* pPreviousLine;
	GCommand* pNextLine;

	char name[16];

	int id;
};
struct GConsole
{
	int show_percentage;
	bool show;

	int cursor_blink;
	bool cursor_show;

	char buffer[CONSOLE_Y][CONSOLE_X+1];

	int posx, posy;
	int limx, limy;

	GCommand* pHeadCommand;
	GCommand* curCommand;

	int same_part;

	int cmd_id;
	bool cmdProcessed;
};


GCommand* newLineCommand(GCommand* cmd, const char* name, int id);
GCommand* newRowCommand(GCommand* cmd, const char* name, int id);

GConsole* initConsole();
void toggleConsole(GConsole* c);
void render(GConsole* c, GFont* f);
int getCommandID(GConsole* c);
void show_hint(GConsole* c);
void scrollup_console(GConsole* c);
void showConsoleBuffer(GConsole* c, GFont* f, int x, int y);
void putchar_console(GConsole* c, char s);
void backspace_console(GConsole* c);
void putline_console(GConsole* c, const char* fmt,...);
void input_console(GConsole* c, SDL_Event* event);
void clearline_console(GConsole* c);
void setCommandTree(GConsole* c, GCommand* first);
int getCommand(GConsole* c);
void deleteCommand(GCommand* c);
void releaseConsole(void* c);

#endif
