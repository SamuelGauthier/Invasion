#include "invBase.h"

//------------------------------------------------
// invContext
//------------------------------------------------
invContext::invContext(int w, int h, bool fs) : width(w), height(h), fullscreen(fs)
{
}

invContext::~invContext()
{
}

bool invContext::create(const char* title, const char* iconfile)
{
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
	
	// Set GL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Set title and icon
	if(title)
		SDL_WM_SetCaption(title, title);

	if(iconfile){
		SDL_Surface* icon = IMG_Load(iconfile);
		if(icon){
			SDL_WM_SetIcon(icon, NULL);
		}
		SDL_FreeSurface(icon);
	}

	// Setup a video mode
	if(!SDL_SetVideoMode(width, height, 32, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN:0))){
		fprintf(stderr, "Could not set GL mode : %s\n", SDL_GetError());
		return false;
	}

	if(fullscreen)
		SDL_WM_ToggleFullScreen(NULL);
	
	SDL_EnableKeyRepeat(100, 40);
	SDL_EnableUNICODE(1);

	glEnable(GL_DEPTH_TEST);
	return true;
}

void invContext::setFocus(bool f)
{
	if(f)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	else
		SDL_WM_GrabInput(SDL_GRAB_OFF);
}

void invContext::setBackground(float r, float g, float b)
{
	glClearColor(r, g, b, 1.f);
}

void invContext::destroy()
{
	SDL_Quit();
}

void invContext::clearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void invContext::flip()
{
	glFlush();
	SDL_GL_SwapBuffers();
}

float invContext::secs()
{
	return (float)SDL_GetTicks()/1000.f;
}

//------------------------------------------------
// invFlow
//------------------------------------------------
invFlow::invFlow()
{
	events = NULL;
	eventstail = NULL;
	waitinglist = NULL;
	waitinglisttail = NULL;

	continue_loop = true;
	elapsedTime = NULL;
	always = true;
	every_10l = false;
}

invFlow::~invFlow()
{
	while(events)
	{
		invEvent* tmp = events;
		events = events->pNext;

		delete tmp;
	}
	
	while(waitinglist)
	{
		invEvent* tmp = waitinglist;
		waitinglist = waitinglist->pNext;

		delete tmp;
	}
}

void invFlow::add_event(invEvent* e, EVENT_PLACE pos, const char* eventname)
{
	e->pNext = NULL;
	if(pos)
	{
		/*
		e->value[7] = pos;
		strncpy(e->searchname, eventname, 32);

		// first in first out
		if(!waitinglist)
		{
			waitinglist = e;
			waitinglisttail = e;
		}
		else
		{
			waitinglisttail->pNext = e;
			waitinglisttail = e;
		}
		*/
	}
	else
	{
		if(!events)
		{
			events = e;
			eventstail = e;
		}
		else
		{
			eventstail->pNext = e;
			eventstail = e;
		}
	}
}

void invFlow::add(bool* condition, const char* name, void (*fn)(), EVENT_PLACE pos, const char* eventname)
{
	invEvent* e = new invEvent;
	e->fn = (void*)fn;
	e->condition = condition;
	
	add_event(e, pos, eventname);
}

void invFlow::stop()
{	
	continue_loop = false;
}

void invFlow::loop()
{
	invEvent* itevents;
	Uint32 before = 0, after;
	Uint8 inc = 0;

	while(continue_loop) {
		after = SDL_GetTicks();
		if(compensator <= (after - before))
		{
			// every 10 loop
			if(++inc == 10) {
				every_10l = true;
				inc = 0;
			}
			else
				every_10l = false;
			
			// compute ElapsedTime
			if(elapsedTime)
				*elapsedTime = (float)(after-before)/1000.f;

			// process event
			itevents = events;
			while(itevents && continue_loop) {
				processEvent(itevents);
				itevents = itevents->pNext;
			}
			before = after;
	}
	else
		SDL_Delay(compensator - (after - before));
	}
}

void invFlow::processEvent(invEvent* e)
{
	if(*e->condition)
		((*(void (*)())e->fn))();
}

void invFlow::setElapsedTime(float* ptr)
{
	elapsedTime = ptr;
}
