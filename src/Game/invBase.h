#ifndef INVBASE_H
#define INVBASE_H

/*
	TODO:

	manage font with invContext
	waiting list event
	put name in event
	replace SDL
*/
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>


class invContext
{
public:
	int width;
	int height;
	bool fullscreen;

	invContext(int w, int h, bool fs = false);
	~invContext();

	bool create(const char* title = "invGame", const char* iconfile = NULL);
	float secs();
	void setFocus(bool f);
	void setBackground(float r, float g, float b);
	void clearScreen();
	void flip();
	void destroy();
};

enum EVENT_PLACE
{
	LAST = 0,
	BEFORE,
	AFTER,
};

class invEvent
{
public:
	void* fn;
	char name[32];
	char searchname[32];

	bool* condition;

	invEvent* pNext;
};

class invFlow // evt. rename in "invFlow"
{
public:
	invEvent* events;
	invEvent* eventstail;
	invEvent* waitinglist;
	invEvent* waitinglisttail;


	invFlow();
	~invFlow();

	void add_event(invEvent* e, EVENT_PLACE pos, const char* eventname);
	void add(bool* condition, const char* name, void (*fn)(), EVENT_PLACE pos = LAST, const char* eventname = "");
	void processEvent(invEvent* e);
	void setElapsedTime(float* ptr);
	void stop();
	
	void loop();

	bool continue_loop;
	
	// predefinied conditions
	bool always; // FIXME const type
	bool every_10l;
	Uint32 compensator;
	float* elapsedTime;
};

#endif
