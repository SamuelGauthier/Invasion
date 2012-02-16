#ifndef INVTALK_H
#define INVTALK_H
#define CONSOLE_H
#define CONSOLE_H
#define CONSOLE_X 30
#define CONSOLE_Y 10
#define TEXT_SIZE 24
#define TEXT_DELAY 300
#define CBUFFER (buffer[cons->cury * CONSOLE_X + cons->curx])
#include <string.h>
#include <SDL/SDL.h>

//------------------------------------------------
// Register
//------------------------------------------------
namespace Register
{
	typedef void (*CommandFunction)(char** args, void* data);
	struct Command 
	{
		const char* name;
		const char* args;
		CommandFunction pComFunc;

		void* data;

		Command* pNext;
	}; /* Command */

	enum COMMAND_ERROR
	{
		UNKNOWN_COMMAND = 1,
		INVALID_ARGUMENT,
	};

	extern Command* cmdlist;

	void init();
	void add(const char* name, const char* args, CommandFunction func, void* data);
	Command* find(const char* name);
	COMMAND_ERROR execCommand(const char* line);
	char** getArgs(Command* cmd, int numARg, const char* rest);
	int numArgs(Command* cmd);
	void deinit(void* tmp);
} /* Register */

//------------------------------------------------
// Interface
//------------------------------------------------
namespace Interface 
{
	struct Console 
	{
		int curx, cury;
		int posx, posy;
		int cursorx;
		char buffer[CONSOLE_X*CONSOLE_Y];

		int color[CONSOLE_Y];
		int delay;
		bool show;
	}; /* Console */

	extern Console* cons;

	void init();
	void input();
	void putChar(char c, const bool command);
	void print(const char* str);
	void clear(char**, void* data);
	void echo(char**, void* data);
	void newCommand();
	void scrollUp();
	void render(Text::INV_Font* font);
	void deinit(void* tmp);
} /* Interface */

//------------------------------------------------
// Input
//------------------------------------------------
namespace Input 
{
	extern SDL_Event event;
	extern bool key[256];
	extern Uint8 mouseButton;
	extern Uint16 keychar;
	extern bool quit;
	extern int mousex, mousey;
	void getInput();
} /* Input */
#endif
