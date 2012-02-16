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
		if(cons->cursorx < CONSOLE_X-1)
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
	for(int i=0;i<CONSOLE_X;i++)
		cons->buffer[CONSOLE_X * (CONSOLE_Y-1) + i] ='\0';

	cons->buffer[CONSOLE_X * (CONSOLE_Y-1)] = '$';
	cons->buffer[CONSOLE_X * (CONSOLE_Y-1) + 1] = ' ';
	cons->cursorx = 2;
}

void Interface::render(Text::INV_Font* font)
{
	int x=0, y=730;

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
		glVertex3i(x + CONSOLE_X * 15,  y, -1);
		glVertex3i(x + CONSOLE_X * 15, y + font->h, -1);
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
			glVertex3i(x + CONSOLE_X * 15,  y, -1);
			glVertex3i(x + CONSOLE_X * 15, y + font->h, -1);
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
// Input
//------------------------------------------------
SDL_Event Input::event;
bool Input::key[256] = {false};
Uint8 Input::mouseButton = 0;
Uint16 Input::keychar = 0;
bool Input::quit = false;
int Input::mousex = 0, Input::mousey;

void Input::getInput()
{
	keychar = 0;
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
		}
	}

	mouseButton = SDL_GetMouseState(&mousex, &mousey);
}
