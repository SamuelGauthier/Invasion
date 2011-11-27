#ifndef TIMER_H
#define TIMER_H
#include "utils.h"
#include "garbage_collector.h"

struct GTimer
{
	LARGE_INTEGER freq;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
};

GTimer* initTimer();
void start(GTimer* t);
float lap(GTimer* t);
void releaseTimer(void* t);

#endif