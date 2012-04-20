#ifndef INVTALK_H
#define INVTALK_H
#define CONSOLE_H
#define CONSOLE_H
#define CONSOLE_COMMAND_X 80
#define CONSOLE_X 40
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
		char buffer[CONSOLE_X*(CONSOLE_Y-1) + CONSOLE_COMMAND_X];

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
// GUI
//------------------------------------------------
namespace GUI 
{
	typedef void (*event_function)(void);
	enum CONTROL_TYPE
	{
		WINDOW = 0,
		BUTTON,
		PANEL,
		LIST,
		TEXTBOX,
		LABEL,
	};

	struct Control
	{
		char* text;
		int value;
		bool clickable;
		int x,y;
		int w,h;
		int type;
		
		event_function onclick;
		Control* pChild[16];
		int numChild;
	};

	extern Control* window;
	extern bool focusedGUI;
	static Control* selected = NULL;

	void init();
	Control* newControl(Control* parent, int x, int y, int w, int h);
	Control* addButton(Control* parent, const char* text, int x, int y, event_function onclick, int w = 80, int h = 20);
	Control* addPanel(Control* parent, int x, int y, int w, int h);
	Control* addList(Control* parent, char* list, int x, int y, int w = 80, int h = 20);
	Control* addTextBox(Control* parent, const char* text, int x, int y, int w = 120, int h = 20);
	Control* addLabel(Control* parent, const char* text, int x, int y, int w = 80, int h = 20);
	void render(Control* c, Text::INV_Font* font, int level);
	void interact(Control* c);
	void parentTo(Control* parent, Control* child);
	void unparentOf(Control* parent, Control* child);
	bool focused(Control* c);
	char* selectedList(Control* list, char** newline);
	void removeList(Control* list, char* name);

	void deinit(void*);
} /* GUI */
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
	extern Uint8 mouseButtonStates;
	void getInput();
} /* Input */
#endif
