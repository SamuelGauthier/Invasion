#include "timer.h"

GTimer* initTimer()
{
	GTimer* t = new GTimer;

	QueryPerformanceFrequency(&t->freq);

	if(!t->freq.QuadPart)
	{
		delete t;
		fprintf(stderr, "Cannot initialize high precision counter\n");
		exit(0);
	}

	t->freq.QuadPart /= 1000.0;

	addRelease(releaseTimer, (void*)t);

	return t;
}

void start(GTimer* t)
{
	QueryPerformanceCounter(&t->start);
}

float lap(GTimer* t)
{
	QueryPerformanceCounter(&t->end);
	return (float)((double)(t->end.QuadPart - t->start.QuadPart) / (double)t->freq.QuadPart);
}

void releaseTimer(void* t)
{
	GTimer* tmp = (GTimer*)t;
	delete tmp;
}
