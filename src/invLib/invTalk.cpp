#include "invMath.h"
#include "invTex.h"
#include "invBase.h"
#include "invTalk.h"

//------------------------------------------------
// Register
//------------------------------------------------
Register::Command* Register::cmdlist;
void Register::init()
{
	cmdlist = NULL;
	GarbageCollector::add(deinit, (void*)cmdlist);
}

void Register::add(const char* name, const char* args, CommandFunction func, void* data)
{
	Command* cmd = new Command;

	cmd->name = name;
	cmd->args = args;
	cmd->data = data;
	cmd->pComFunc = func;

	cmd->pNext = cmdlist;
	cmdlist = cmd;

}

Register::Command* Register::find(const char* name)
{
	if(cmdlist)
	{
		Command* pCopy = cmdlist;
		while(pCopy)
		{
			if(!strcmp(pCopy->name, name))
				return pCopy;
			pCopy = pCopy->pNext;
		}
	}
	return NULL;
}


Register::COMMAND_ERROR Register::execCommand(const char* line)
{
	char cmdname[16];
	int cmdlength = (int)strcspn(line, " \0");
	const char* rest = line + cmdlength;
	strncpy(cmdname, line, cmdlength);
	cmdname[cmdlength] = '\0';
	Command* cmd = find(cmdname);
	if(!cmd)
		return UNKNOWN_COMMAND;
	else
	{
		int numArg = numArgs(cmd);
		char** args = getArgs(cmd, numArg, rest);
		if(!args)
			return INVALID_ARGUMENT;
		else
		{
			cmd->pComFunc(args, cmd->data);

			// delete args
			for(int i=0;i<numArg;i++)
				delete[] args[i];
			delete[] args;
		}
	}
}
char** Register::getArgs(Command* cmd, int numArg, const char* rest)
{
	int i = 0;
	char** args = new char*[numArg];
	while(*rest)
	{
		if(i == numArg)
		{
			i++;
			break;
		}

		rest++;

		if(cmd->args[i] == 's')
		{
			const int length = strlen(rest);
			args[i] = new char[length+1];

			strncpy(args[i], rest, length);
			args[i][length] = '\0';

			rest+=length;
		}
		else
		{
			const int length_arg = (int)strcspn(rest, " \0");
			args[i] = new char[length_arg + 1];

			strncpy(args[i], rest, length_arg);
			args[i][length_arg] = '\0';
			
			rest+=length_arg;
		}

		i++;
	}

	if(i != numArg)
	{
		if(numArg)
		{
			for(int j=0;j<i;j++)
				delete[] args[j];
			delete[] args;
		}
		args = NULL;
	}

	return args;
}

int Register::numArgs(Command* cmd)
{
	return strlen(cmd->args);
}

void Register::deinit(void* tmp)
{
	Command* cmdlist = (Command*)tmp;
	while(cmdlist)
	{
		Command* todelete = cmdlist;
		cmdlist = cmdlist->pNext;

		delete todelete;
	}
	delete cmdlist;

}

//------------------------------------------------
// Interface
//------------------------------------------------
Interface::Console* Interface::cons;
void Interface::init()
{
	Register::init();
	cons = new Console;
	cons->curx = 0;
	cons->cury = 0;
	cons->posx = 0;
	cons->posy = 0;
	cons->show = false;
	clear(NULL, NULL);
	newCommand();

	// add some basic commands
	Register::add("clear", "", clear, (void*)cons);
	Register::add("echo", "s", echo, NULL);

	int i=0;
	for (i = 0; i < CONSOLE_Y; i++)
		cons->color[i] = 0xFFFFFF;
}

void Interface::input()
{
	if(Input::keychar > 255)
		return;
	char c = Input::keychar;
	char* buffer = cons->buffer;
	if(c == 8)
	{
		if(cons->cursorx > 2)
		{
			cons->cursorx--;
			buffer[(CONSOLE_Y-1)*CONSOLE_X + cons->cursorx] = 0;
		}
	}

	else if(c == 13)
	{
		if(cons->cursorx > 2)
		{
			int result = Register::execCommand(&buffer[(CONSOLE_Y-1) * CONSOLE_X + 2]);
			if(result)
			{
				/* TODO : Error handling */
			}
			newCommand();
		}
	}

	else if(c >= 32)
	{
		putChar(c, true);
	}

	cons->delay = TEXT_DELAY;
}

void Interface::putChar(char c, const bool command)
{
	char* buffer = cons->buffer;

	if(command)
	{
		if(cons->cursorx < CONSOLE_COMMAND_X-2)
		{
			buffer[cons->cursorx + CONSOLE_X * (CONSOLE_Y-1)] = c;
			cons->cursorx++;
		}
	}

	else
	{
		if(c == '\n')
		{
			cons->cury++;
			cons->curx = 0;
		}
		else if(cons->curx < CONSOLE_X-1)
		{
			CBUFFER = c;
			cons->curx++;
		}
	}
}

void Interface::print(const char* str)
{
	if(cons->cury == CONSOLE_Y - 1)
	{
		scrollUp();
		cons->cury--;
	}
	while(*str)
		putChar(*str++, false);
	putChar('\n', false);
}

void Interface::clear(char**, void* data)
{
	char* buffer = cons->buffer;
	for(int i=0;i<CONSOLE_X*CONSOLE_Y-1;i++)
		buffer[i] = '\0';

	for(int i=0;i<CONSOLE_Y-1;i++)
		cons->color[i] = 0xFFFFFF;
	cons->curx = 0; cons->cury = 0;
}

void Interface::echo(char** args, void*)
{
	print(args[0]);
}

void Interface::scrollUp()
{
	char* buffer = cons->buffer;
	for(int i=1;i<CONSOLE_Y-1;i++)
	{
		for(int j=0;j<CONSOLE_X;j++)
			buffer[(i-1)*CONSOLE_X + j] = buffer[i*CONSOLE_X + j];
		cons->color[i-1] = cons->color[i];
	}

	for(int i=0;i<CONSOLE_X;i++)
	{
		buffer[(CONSOLE_Y-2) * CONSOLE_X + i] = '\0';
	}
}

void Interface::newCommand()
{
	for(int i=0;i<CONSOLE_COMMAND_X;i++)
		cons->buffer[CONSOLE_X * (CONSOLE_Y-1) + i] ='\0';

	cons->buffer[CONSOLE_X * (CONSOLE_Y-1)] = '$';
	cons->buffer[CONSOLE_X * (CONSOLE_Y-1) + 1] = ' ';
	cons->cursorx = 2;
}

void Interface::render(Text::INV_Font* font)
{
	int x = 0, y = Game::height - 20;
	if(cons->cury)
	{
		if(!cons->delay)
		{
			scrollUp();
			cons->cury--;
			cons->delay = TEXT_DELAY/4;
		}
		cons->delay--;
	}

	if(cons->show)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

		glColor4f(1.f,1.f,1.f,0.3f);
		glBegin(GL_QUADS);
		glVertex3i(x, y, -1);
		glVertex3i(x + CONSOLE_COMMAND_X * font->w,  y, -1);
		glVertex3i(x + CONSOLE_COMMAND_X * font->w, y + font->h, -1);
		glVertex3i(x, y + font->h, -1);
		glEnd();
		glDisable(GL_BLEND);

		glColor3f(1.f, 1.f, 1.f);

		// TODO : render the cursor a little bit better
		cons->buffer[(CONSOLE_Y-1) * CONSOLE_X + cons->cursorx] = '_';
		Text::renderText(&cons->buffer[(CONSOLE_Y-1) * CONSOLE_X], cons->posx + x, cons->posy + y, font);
		cons->buffer[(CONSOLE_Y-1) * CONSOLE_X + cons->cursorx] = '\0';
	}
	y-=4*font->h;

	for(int i=CONSOLE_Y-2;i>=0;i--)
	{
		if(cons->buffer[i * CONSOLE_X])
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

			glColor4f(0.f,0.f,0.f,0.3f);
			glBegin(GL_QUADS);

			glVertex3i(x, y, -1);
			glVertex3i(x + CONSOLE_X * font->w,  y, -1);
			glVertex3i(x + CONSOLE_X * font->w, y + font->h, -1);
			glVertex3i(x, y + font->h, -1);

			glEnd();
			glDisable(GL_BLEND);

			glColor3f((float)(cons->color[i]>>16 & 0xFF)/255.f, (float)(cons->color[i]>>8 & 0xFF)/255.f, (float)(cons->color[i] & 0xFF)/255.f);
			Text::renderText(&cons->buffer[i * CONSOLE_X], cons->posx + x, cons->posy + y, font);

			y-=font->h;
		}
	}
}

void Interface::deinit(void* tmp)
{
	Console* cons = (Console*)tmp;
	delete cons;
}

//------------------------------------------------
// GUI
//------------------------------------------------
GUI::Control* GUI::window = NULL;
bool GUI::focusedGUI = false;

GUI::Control* GUI::newControl(Control* parent, int x, int y, int w, int h)
{
	Control* c = new Control;
	c->text = NULL;
	c->value = 0;
	c->clickable = false;
	if(parent){
		c->x = parent->x + x; c->y = parent->y + y;
	}
	else{
		c->x = x; c->y = y;
	}
	c->w = w; c->h = h;
	c->type = WINDOW;

	c->onclick = NULL;
	c->numChild = 0;

	if(parent)
		parent->pChild[parent->numChild++] = c;
	return c;
}

GUI::Control* GUI::addButton(Control* parent, const char* text, int x, int y, event_function onclick, int w, int h)
{
	Control* c = newControl(parent, x,y,w,h);
	c->text = new char[strlen(text) + 1];
	strncpy(c->text, text, strlen(text));
	c->text[strlen(text)] = '\0';
	c->clickable = true;
	c->onclick = onclick;
	c->type = BUTTON;
	return c;
}

GUI::Control* GUI::addPanel(Control* parent, int x, int y, int w, int h)
{
	Control* c = newControl(parent, x, y, w, h);
	c->type = PANEL;
	return c;
}

GUI::Control* GUI::addList(Control* parent, char* text, int x, int y, int w, int h)
{
	Control* c = newControl(parent, x, y, w, h);
	c->text = new char[strlen(text) + 1];
	strncpy(c->text, text, strlen(text));
	c->text[strlen(text)] = '\0';
	c->clickable = true;
	c->type = LIST;
	return c;
}

GUI::Control* GUI::addTextBox(Control* parent, const char* text, int x, int y, int w, int h)
{
	Control* c = newControl(parent, x, y, w, h);
	c->text = new char[12];
	strncpy(c->text, text, strlen(text));
	c->text[strlen(text)] = '\0';
	c->clickable = true;
	c->type = TEXTBOX;
	return c;
}

GUI::Control* GUI::addLabel(Control* parent, const char* text, int x, int y, int w, int h)
{
	Control* c = newControl(parent, x, y, w, h);
	c->text = new char[strlen(text) + 1];
	strncpy(c->text, text, strlen(text));
	c->text[strlen(text)] = '\0';
	c->type = LABEL;
	return c;
}

void GUI::init()
{
	window = new Control;
	window->x = 0;
	window->y = 0;
	window->w = Game::width;
	window->h = Game::height;
	window->onclick = NULL;
	window->clickable = false;
	window->type = WINDOW;
	window->numChild = 0;
	window->text = NULL;
	GarbageCollector::add(deinit, window);
}

void GUI::parentTo(Control* parent, Control* child)
{
	unparentOf(parent, child);
	parent->pChild[parent->numChild++] = child;	
	child->x += parent->x; child->y += parent->y;
}

void GUI::unparentOf(Control* parent, Control* child)
{
	for(int i=0;i<parent->numChild;i++)
	{
		if(parent->pChild[i] == child)
		{
			for(int j=i;j<parent->numChild-1;j++)
				parent->pChild[j] = parent->pChild[j+1];
			parent->pChild[--parent->numChild] = NULL;
			child->x -= parent->x; child->y -= parent->y;
			break;
		}
	}
}

void GUI::deinit(void* tmp)
{
	Control* c = (Control*)tmp;
	for(int i=0;i<c->numChild;i++)
	{
		deinit((void*)c->pChild[i]);
	}

	if(c->text)
		delete[] c->text;
	delete c;
}

char* GUI::selectedList(Control* c, char** newline)
{
	int index = 0;
	char* str = c->text, *tmp = 0;
	while(index < c->value)
	{
		tmp = strchr(str, '\n') + 1;
		if(*tmp == '\0'){
			c->value = index;
			break;
		}

		str = tmp;
		index++;
	}
	*newline = strchr(str, '\n');
	**newline = '\0';

	return str;
}

void GUI::render(Control* c, Text::INV_Font* font, int level)
{
	switch(c->type)
	{
		case BUTTON:
		case TEXTBOX:
			if(c->type == BUTTON)
				glColor3ub(-level * 20, -level * 20, -level * 20);
			else if(c->type == TEXTBOX)
				glColor3ub(0, 0, 0);
			glBegin(GL_QUADS);
			glVertex3i(c->x, c->y, level);
			glVertex3i(c->x + c->w, c->y, level);
			glVertex3i(c->x + c->w, c->y + c->h, level);
			glVertex3i(c->x, c->y + c->h, level);
			glEnd();
		case LABEL:
			glColor3f(1.f, 1.f, 1.f);
			Text::renderText(c->text, c->x + 8, c->y + 2, font);
			break;
		case PANEL:
			glColor3ub(-level * 20, -level * 20, -level * 20);
			glBegin(GL_QUADS);
			glVertex3i(c->x, c->y, level);
			glVertex3i(c->x + c->w, c->y, level);
			glVertex3i(c->x + c->w, c->y + c->h, level);
			glVertex3i(c->x, c->y + c->h, level);
			glEnd();
			break;
		case LIST:
			glColor3ub(0, 0, 0);
			glBegin(GL_QUADS);
			glVertex3i(c->x, c->y, level);
			glVertex3i(c->x + c->w, c->y, level);
			glVertex3i(c->x + c->w, c->y + c->h, level);
			glVertex3i(c->x, c->y + c->h, level);
			glEnd();
			if(*c->text)
			{
				char* newline;
				char* str = selectedList(c, &newline);
				glColor3f(1.f, 1.f, 1.f);
				Text::renderText(str, c->x + 8, c->y + 2, font);
				*newline = '\n';
			}
			break;
	}

	for(int i=0;i<c->numChild;i++)
		render(c->pChild[i], font, level+1);
}

void GUI::interact(Control* c)
{
	if(focusedGUI)
	{
		if((Input::mouseButton & SDL_BUTTON(1)) && 
			(Input::mouseButtonStates & 1))
		{
			selected = NULL;
			if(c->clickable &&
			   c->x <= Input::mousex && c->x + c->w >= Input::mousex &&
			   c->y <= Input::mousey && c->y + c->h >= Input::mousey)
			{
				selected = c;
				if(c->onclick)
					(*c->onclick)();
			}
			else
			{
				for(int i=0;i<c->numChild && !selected;i++)
					interact(c->pChild[i]);
			}
		}

		else if(selected && Input::keychar)
		{
			if(selected->type == LIST)
			{
				if(Input::keychar == SDLK_DOWN)
					selected->value++;
				else if(Input::keychar == SDLK_UP && selected->value)
					selected->value--;
			}

			else if(selected->type == TEXTBOX)
			{
				int length = strlen(selected->text);
				if(Input::keychar < 128 && Input::keychar >= 32 && length < 12)
				{
					selected->text[length] = Input::keychar;
					selected->text[length+1] = '\0';
				}

				else if(Input::keychar == 8 && length)
				{
					selected->text[length-1] = '\0';
				}
			}
		}
	}
}

bool GUI::focused(Control* parent)
{
	focusedGUI = false;
	for(int i=0;i<parent->numChild;i++)
	{
		Control* c = parent->pChild[i];
		if(c->x <= Input::mousex && c->x + c->w >= Input::mousex &&
		   c->y <= Input::mousey && c->y + c->h >= Input::mousey)
		{
			focusedGUI = true;
			break;
		}
	}
}
//------------------------------------------------
// Input
//------------------------------------------------
SDL_Event Input::event;
bool Input::key[256] = {false};
Uint8 Input::mouseButton = 0;
Uint8 Input::mouseButtonStates;
Uint16 Input::keychar = 0;
bool Input::quit = false;
int Input::mousex = 0, Input::mousey;

void Input::getInput()
{
	keychar = 0;
	mouseButtonStates = 0;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym < 256)
					key[event.key.keysym.sym] = true;
				if(!event.key.keysym.unicode)
					keychar = (Uint16)event.key.keysym.sym;
				else
					keychar = (Uint16)event.key.keysym.unicode;
				break;
			case SDL_KEYUP:
				if(event.key.keysym.unicode < 256)
					key[event.key.keysym.sym] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseButtonStates |= 1;
				break;
		}
	}
	mouseButton = SDL_GetMouseState(&mousex, &mousey);
}
