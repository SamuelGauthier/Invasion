#include "console.h"

GConsole* initConsole()
{
	GConsole* c = new GConsole;
	c->show_percentage = 0;
	c->show = false;
	c->cursor_show = true;
	c->cursor_blink = 25;

	c->curCommand = 0;
	c->pHeadCommand = 0;
	c->cmd_id = 0;

	for(int i=0;i<CONSOLE_Y;i++)
	{
		for(int j=0;j<CONSOLE_X+1;j++)
		{
			c->buffer[i][j] = '\0';
		}
	}

	c->posx = 0;
	c->posy = 0;

	putline_console(c, "RTS Editor v0.0.1 pre-pre-alpha (18.11.2011)\n");
	putline_console(c, "by TheAslan and Jyuki\n\n");
	putline_console(c, "> ");
	c->limx = c->posx;
	c->limy = c->posy;

	addRelease(releaseConsole, (void*)c);
	return c;
}

void toggleConsole(GConsole* c)
{
	c->show = !c->show;
}

void render(GConsole* c, GFont* f)
{
	if(c->show || c->show_percentage)
	{
		if(c->show_percentage != CONSOLE_Y*14 && c->show)
		{
			c->show_percentage+=(CONSOLE_Y*14 - c->show_percentage)/5 + 1;
		}
		
		else if(c->show_percentage != 0 && !c->show)
		{
			c->show_percentage-=80;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

		glColor4f(0.f,0.f,0.f,0.05f);
		glBegin(GL_QUADS);

		glVertex3i(0, c->show_percentage - (CONSOLE_Y * 14) , -1);
		glVertex3i(800, c->show_percentage - (CONSOLE_Y * 14) , -1);
		glVertex3i(800, c->show_percentage, -1);
		glVertex3i(0, c->show_percentage, -1);

		glEnd();
		glDisable(GL_BLEND);

		glColor3f(1.f, 1.f, 1.f);
		showConsoleBuffer(c, f, 0, c->show_percentage - CONSOLE_Y * 14);

		if(!c->cursor_blink)
		{
			c->cursor_show = !c->cursor_show;
			c->cursor_blink = 25;
		}
		else
		{
			c->cursor_blink--;
		}

		if(c->cursor_show)
		{
			render(c->posx * 8 - 3, c->show_percentage - CONSOLE_Y*14 + c->posy * 14, f, "|");
		}
	}
}

void input_console(GConsole* c, SDL_Event* event)
{
	if(event->key.keysym.unicode >= 32)
		putchar_console(c, (char)event->key.keysym.unicode);

	if(event->key.keysym.unicode == 8)
		backspace_console(c);
	if(event->key.keysym.unicode == 13)
	{
		c->cmd_id = getCommandID(c);
	}

	c->cursor_blink = 25;
	c->cursor_show = true;

	show_hint(c);
}

int getCommandID(GConsole* c)
{
	char* s = &c->buffer[c->limy][c->limx];
	GCommand* pCopy = c->curCommand;
	int id = 1;

	while(*s && pCopy)
	{
		// Enlève les espaces
		while(*s == ' '){s++;}

		int word = 0;
		char* begin = s;
		while(*s && *s != ' ')
		{
			s++;
			word++;
		}

		bool found = false;
		while(pCopy)
		{
			if(strncmp(pCopy->name, begin, word) == 0)
			{
				if(pCopy->pNextLine)
					pCopy = pCopy->pNextLine;
				found = true;
				break;
			}
			pCopy = pCopy->pNextRow;
		}

		if(!found)
			break;

		// si c'est une commande terminal
		if(!pCopy->pNextLine && !(*s))
		{
			id = pCopy->id;
			break;
		}
	}

	return id;
}

void show_hint(GConsole* c){

	GCommand* pCopy = c->pHeadCommand;

	char* s = &c->buffer[c->limy][c->limx];

	// On efface la ligne
	int tmpx = c->posx, tmpy = c->posy;
	c->posx = 0; c->posy = 29;

	clearline_console(c);

	int hint_num = 4;
	if(!(*s))
	{
		c->same_part = 0;
		while(pCopy && hint_num)
		{
			putline_console(c, pCopy->name);
			putchar_console(c, ' ');
			pCopy = pCopy->pNextRow;
			hint_num--;
		}
	}

	while(*s && pCopy){

		while(*s == ' '){s++;}

		char* begin = s;
		int word = 0;
		while(*s && *s != ' '){
			s++;
			word++;
		}

		if(!(*s))
		{
			c->same_part = word;
			while(pCopy && hint_num)
			{
				if(!strncmp(pCopy->name, begin, word))
				{
					putline_console(c, pCopy->name);
					putchar_console(c, ' ');
					hint_num--;
				}
				pCopy = pCopy->pNextRow;
			}

		}

		else /* *s == ' ' */
		{
			while(pCopy)
			{
				if(!strncmp(pCopy->name, begin, word))
				{
					pCopy = pCopy->pNextLine;
					break;
				}
				pCopy = pCopy->pNextRow;
			}
		}
	}
	c->posx = tmpx; c->posy = tmpy;
}

void scrollup_console(GConsole* c)
{
	for(int i=0;i<CONSOLE_Y-2;i++)
	{
		for(int j=0;j<CONSOLE_X;j++)
		{
			c->buffer[i][j] = c->buffer[i+1][j];
		}
	}

	for(int i=0;i<CONSOLE_X;i++)
	{
		c->buffer[28][i] = '\0';
	}

}

void showConsoleBuffer(GConsole* c, GFont* f, int x, int y)
{
	for(int i=0;i < CONSOLE_Y;i++)
	{
		renderMono(x, y + i * 14, f, "%s", &c->buffer[i][0]);
	}

	// affiche les points communs en jaune
	// char* s = c->buffer
}

void putchar_console(GConsole* c, char s)
{
	c->buffer[c->posy][c->posx] = s;

	c->posx++;
	if(c->posx == CONSOLE_X)
	{
		c->posy++;
		if(c->posy == CONSOLE_Y - 1)
		{
			scrollup_console(c);
			c->posy--;
		}
		c->posx = 0;
	}
}

void backspace_console(GConsole* c)
{
	if((c->posx > c->limx && c->posy == c->limy) || c->posy > c->limy)
	{
		if(!c->posx)
		{
			c->posx = CONSOLE_X - 1;
			c->posy--;
		}
		else
			c->posx--;

		c->buffer[c->posy][c->posx] = '\0';
	}
}

int getCommand(GConsole* c)
{
	int a = c->cmd_id;
	c->cmd_id = 0;

	return a;
}

void putline_console(GConsole* c, const char* fmt,...)
{
	char text[128];
	va_list ap;
	
	va_start(ap, fmt);
	vsprintf(text, fmt,ap);
	va_end(ap);

	int index = 0;
	while(text[index])
	{
		char s = text[index];

		if(s == '\n')
		{
			if(c->posy == CONSOLE_Y - 2)
				scrollup_console(c);
			else
				c->posy++;
			c->posx = 0;
		}
		else
		{
			putchar_console(c, s);
		}

		index++;
	}
}

void setCommandTree(GConsole* c, GCommand* first)
{
	c->pHeadCommand = first;
	c->curCommand = first;

	show_hint(c);
}

GCommand* newLineCommand(GCommand* cmd, const char* name, int id)
{
	GCommand* c = new GCommand;
	
	c->id = id;
	strncpy(c->name, name, 16);
	c->pNextLine = 0;
	c->pNextRow = 0;
	c->pPreviousLine = cmd;

	if(cmd)
	{
		cmd->pNextLine = c;
	}

	return c;
}

GCommand* newRowCommand(GCommand* cmd, const char* name, int id)
{
	GCommand* c = new GCommand;
	
	c->id = id;
	strncpy(c->name, name, 16);

	c->pNextLine = 0;
	c->pNextRow = 0;

	if(cmd)
	{
		while(cmd->pNextRow)
		{
			cmd = cmd->pNextRow;
		}
		cmd->pNextRow = c;
		c->pPreviousLine = cmd->pPreviousLine;
	}
	return c;
}

void clearline_console(GConsole* c)
{
	char* begin = &c->buffer[c->posy][0];
	for(int i=0;i<CONSOLE_X;i++)
		begin[i] = '\0';
	c->posx = 0;
}

void deleteCommand(GCommand* c)
{
	if(c){
		if(c->pNextLine)
			deleteCommand(c->pNextLine);
		if(c->pNextRow)
			deleteCommand(c->pNextRow);
		delete c;
	}
}

void releaseConsole(void* c)
{
	GConsole* tmp = (GConsole*)c;
	deleteCommand(tmp->pHeadCommand);
	delete tmp;
}
